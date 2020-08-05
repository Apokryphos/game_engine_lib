#include "render_vk/command_buffer.hpp"
#include "render_vk/command_pool.hpp"
#include "render_vk/debug_utils.hpp"
#include "render_vk/descriptor_set_layout.hpp"
#include "render_vk/model_manager.hpp"
#include "render_vk/renderers/spine_sprite_renderer.hpp"
#include "render_vk/shader.hpp"
#include "render_vk/texture.hpp"
#include "render_vk/vertex.hpp"
#include "render_vk/vulkan_model.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <cassert>
#include <spine/spine.h>
#include <vector>

using namespace render;
using namespace spine;

namespace render_vk
{
//  ----------------------------------------------------------------------------
static void create_sprite_pipeline(
    VkDevice device,
    const VulkanSwapchain& swapchain,
    VkRenderPass render_pass,
    const VkSampleCountFlagBits msaa_sample_count,
    const DescriptorSetLayouts& descriptor_set_layouts,
    VkPipelineLayout& pipeline_layout,
    VkPipeline& pipeline
);

//  ----------------------------------------------------------------------------
static void draw_spine_sprite_batch(SpineSpriteBatch batch) {
    assert(batch.skeleton != nullptr);

    spine::Skeleton* skeleton = batch.skeleton;

    const auto& slots = skeleton->getSlots();
    const auto slot_count = slots.size();

    for (int s = 0; s < slot_count; ++s) {
        Slot* slot = skeleton->getDrawOrder()[s];

        Attachment* attachment = slot->getAttachment();
        if (attachment == nullptr) {
            continue;
        }

        Texture* texture = NULL;
        unsigned short* indices = NULL;

        Color skeleton_color = skeleton->getColor();
        Color slot_solor = slot->getColor();
        Color tint(
            skeleton_color.r * slot_solor.r,
            skeleton_color.g * slot_solor.g,
            skeleton_color.b * slot_solor.b,
            skeleton_color.a * slot_solor.a
        );

        spine::Vector<Vertex> vertices;
        unsigned short quadIndices[] = {0, 1, 2, 2, 3, 0};

        if (attachment->getRTTI().isExactly(RegionAttachment::rtti)) {
            RegionAttachment* regionAttachment = (RegionAttachment*)attachment;

            // Our engine specific Texture is stored in the AtlasRegion which was
            // assigned to the attachment on load. It represents the texture atlas
            // page that contains the image the region attachment is mapped to.
            texture = (Texture*)((AtlasRegion*)regionAttachment->getRendererObject())->page->getRendererObject();

            // Ensure there is enough room for vertices
            vertices.setSize(4, Vertex());

            // Computed the world vertices positions for the 4 vertices that make up
            // the rectangular region attachment. This assumes the world transform of the
            // bone to which the slot (and hence attachment) is attached has been calculated
            // before rendering via Skeleton::updateWorldTransform(). The vertex positions
            // will be written directoy into the vertices array, with a stride of sizeof(Vertex)
            regionAttachment->computeWorldVertices(
                slot->getBone(),
                &vertices.buffer()->position.x,
                0,
                sizeof(Vertex)
            );

            // copy color and UVs to the vertices
            for (size_t j = 0, l = 0; j < 4; j++, l+=2) {
                Vertex& vertex = vertices[j];
                vertex.color = glm::vec3(tint.r, tint.g, tint.b);
                vertex.tex_coord = glm::vec2(
                    regionAttachment->getUVs()[l],
                    regionAttachment->getUVs()[l + 1]
                );
            }

            // set the indices, 2 triangles forming a quad
            indices = quadIndices;
        }
    }
}

//  ----------------------------------------------------------------------------
SpineSpriteRenderer::SpineSpriteRenderer(ModelManager& model_mgr)
: m_model_mgr(model_mgr) {
}

//  ----------------------------------------------------------------------------
void SpineSpriteRenderer::create_objects(
    VkDevice device,
    const VulkanSwapchain& swapchain,
    VkRenderPass render_pass,
    const VkSampleCountFlagBits msaa_sample_count,
    const DescriptorSetLayouts& descriptor_set_layouts
) {
    m_device = device;
    m_render_pass = render_pass;

    create_sprite_pipeline(
        device,
        swapchain,
        render_pass,
        msaa_sample_count,
        descriptor_set_layouts,
        m_pipeline_layout,
        m_pipeline
    );
}

//  ----------------------------------------------------------------------------
void SpineSpriteRenderer::destroy_objects() {
    vkDestroyPipeline(m_device, m_pipeline, nullptr);
    m_pipeline = VK_NULL_HANDLE;

    vkDestroyPipelineLayout(m_device, m_pipeline_layout, nullptr);
    m_pipeline_layout = VK_NULL_HANDLE;

    m_render_pass = VK_NULL_HANDLE;
    m_device = VK_NULL_HANDLE;
}

//  ----------------------------------------------------------------------------
void SpineSpriteRenderer::draw_sprites(
    const std::vector<SpineSpriteBatch>& batches,
    const FrameDescriptorObjects& descriptors,
    VkCommandBuffer command_buffer
) {
    VkCommandBufferInheritanceInfo inherit_info{};
    inherit_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
    inherit_info.renderPass = m_render_pass;

    //  Record secondary command buffer
    VkCommandBufferBeginInfo begin_info{};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags =
        VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT |
        VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
    begin_info.pInheritanceInfo = &inherit_info;

    begin_debug_marker(command_buffer, "Draw Sprites", DEBUG_MARKER_COLOR_ORANGE);
    if (vkBeginCommandBuffer(command_buffer, &begin_info) != VK_SUCCESS) {
        throw std::runtime_error("Failed to begin recording command buffer.");
    }

    //  Check if batches are empty
    if (batches.empty()) {
        if (vkEndCommandBuffer(command_buffer) != VK_SUCCESS) {
            throw std::runtime_error("Failed to record secondary command buffer.");
        }
        end_debug_marker(command_buffer);
        return;
    }

    //  Bind pipeline
    vkCmdBindPipeline(
        command_buffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        m_pipeline
    );

    //  Bind per-frame descriptors
    vkCmdBindDescriptorSets(
        command_buffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        m_pipeline_layout,
        0,
        1,
        &descriptors.frame_set,
        0,
        nullptr
    );

    //  Bind texture descriptors
    vkCmdBindDescriptorSets(
        command_buffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        m_pipeline_layout,
        1,
        1,
        &descriptors.texture_set,
        0,
        nullptr
    );

    //  Get sprite quad
    const VulkanModel& quad = m_model_mgr.get_sprite_quad();

    //  Keep track of model index because of dynamic buffer alignment
    for (const SpineSpriteBatch& batch : batches) {
        //  Bind vertex buffer
        VkBuffer vertex_buffers[] = { quad.get_vertex_buffer() };
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(command_buffer, 0, 1, vertex_buffers, offsets);

        //  Bind index buffer
        vkCmdBindIndexBuffer(
            command_buffer,
            quad.get_index_buffer(),
            0,
            VK_INDEX_TYPE_UINT32
        );

        const uint32_t index_count = quad.get_index_count();

        //  Texture ID
        vkCmdPushConstants(
            command_buffer,
            m_pipeline_layout,
            VK_SHADER_STAGE_FRAGMENT_BIT,
            sizeof(glm::mat4),
            sizeof(uint32_t),
            &batch.texture_id
        );

        //  Draw each object
        for (uint32_t n = 0; n < batch.positions.size(); ++n) {
            glm::mat4 model =
                glm::translate(glm::mat4(1.0f), batch.positions[n]) *
                glm::scale(glm::mat4(1.0f), batch.sizes[n]);

            vkCmdPushConstants(
                command_buffer,
                m_pipeline_layout,
                VK_SHADER_STAGE_VERTEX_BIT,
                0,
                sizeof(glm::mat4),
                &model
            );

            //  Draw
            vkCmdDrawIndexed(
                command_buffer,
                index_count,
                1,
                0,
                0,
                1
            );
        }
    }

    if (vkEndCommandBuffer(command_buffer) != VK_SUCCESS) {
        throw std::runtime_error("Failed to record secondary command buffer.");
    }

    end_debug_marker(command_buffer);
}

//  ----------------------------------------------------------------------------
static void create_sprite_pipeline(
    VkDevice device,
    const VulkanSwapchain& swapchain,
    VkRenderPass render_pass,
    const VkSampleCountFlagBits msaa_sample_count,
    const DescriptorSetLayouts& descriptor_set_layouts,
    VkPipelineLayout& pipeline_layout,
    VkPipeline& pipeline
) {
    //  Shaders
    auto vert_shader_code = read_file("assets/shaders/vk/sprite_vert.spv");
    auto frag_shader_code = read_file("assets/shaders/vk/sprite_frag.spv");

    VkShaderModule vert_shader_module = create_shader_module(device, vert_shader_code);
    VkShaderModule frag_shader_module = create_shader_module(device, frag_shader_code);

    VkPipelineShaderStageCreateInfo vert_shader_stage_info{};
    vert_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vert_shader_stage_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vert_shader_stage_info.module = vert_shader_module;
    vert_shader_stage_info.pName = "main";

    VkPipelineShaderStageCreateInfo frag_shader_stage_info{};
    frag_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    frag_shader_stage_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    frag_shader_stage_info.module = frag_shader_module;
    frag_shader_stage_info.pName = "main";

    VkPipelineShaderStageCreateInfo shader_stages[] = {
        vert_shader_stage_info,
        frag_shader_stage_info
    };

    //  Vertex input
    auto attrib_descs = Vertex::get_attribute_descriptions();
    auto binding_desc = Vertex::get_binding_description();
    VkPipelineVertexInputStateCreateInfo vertex_input_info{};
    vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertex_input_info.vertexAttributeDescriptionCount = static_cast<uint32_t>(attrib_descs.size());
    vertex_input_info.pVertexAttributeDescriptions = attrib_descs.data();
    vertex_input_info.vertexBindingDescriptionCount = 1;
    vertex_input_info.pVertexBindingDescriptions = &binding_desc;

    //  Input assembly
    VkPipelineInputAssemblyStateCreateInfo input_assembly{};
    input_assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    input_assembly.primitiveRestartEnable = VK_FALSE;

    //  Viewport
    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float) swapchain.extent.width;
    viewport.height = (float) swapchain.extent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    //  Scissors
    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = swapchain.extent;

    VkPipelineViewportStateCreateInfo viewport_state{};
    viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewport_state.viewportCount = 1;
    viewport_state.pViewports = &viewport;
    viewport_state.scissorCount = 1;
    viewport_state.pScissors = &scissor;

    //  Rasterizer
    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;
    rasterizer.depthBiasConstantFactor = 0.0f; // Optional
    rasterizer.depthBiasClamp = 0.0f; // Optional
    rasterizer.depthBiasSlopeFactor = 0.0f; // Optional

    //  Multisampling
    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = msaa_sample_count;
    multisampling.minSampleShading = 1.0f; // Optional
    multisampling.pSampleMask = nullptr; // Optional
    multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
    multisampling.alphaToOneEnable = VK_FALSE; // Optional

    //  Color blending
    VkPipelineColorBlendAttachmentState color_blend_attachment{};
    color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    color_blend_attachment.blendEnable = VK_TRUE;
    color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;
    color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    color_blend_attachment.alphaBlendOp = VK_BLEND_OP_SUBTRACT;

    VkPipelineColorBlendStateCreateInfo color_blending{};
    color_blending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    color_blending.logicOpEnable = VK_FALSE;
    color_blending.logicOp = VK_LOGIC_OP_COPY; // Optional
    color_blending.attachmentCount = 1;
    color_blending.pAttachments = &color_blend_attachment;
    color_blending.blendConstants[0] = 0.0f; // Optional
    color_blending.blendConstants[1] = 0.0f; // Optional
    color_blending.blendConstants[2] = 0.0f; // Optional
    color_blending.blendConstants[3] = 0.0f; // Optional

    //  Depth testing
    VkPipelineDepthStencilStateCreateInfo depth_stencil{};
    depth_stencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depth_stencil.depthTestEnable = VK_TRUE;
    depth_stencil.depthWriteEnable = VK_TRUE;
    depth_stencil.depthCompareOp = VK_COMPARE_OP_LESS;
    depth_stencil.depthBoundsTestEnable = VK_FALSE;
    depth_stencil.minDepthBounds = 0.0f; // Optional
    depth_stencil.maxDepthBounds = 1.0f; // Optional
    depth_stencil.stencilTestEnable = VK_FALSE;

    //  Dynamic state
    VkDynamicState dynamic_states[] = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_LINE_WIDTH
    };

    VkPipelineDynamicStateCreateInfo dynamic_state{};
    dynamic_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamic_state.dynamicStateCount = 2;
    dynamic_state.pDynamicStates = dynamic_states;

    std::array<VkDescriptorSetLayout, 2> set_layouts = {
        descriptor_set_layouts.frame,
        descriptor_set_layouts.texture_sampler,
    };

    //  Push constants
    std::array<VkPushConstantRange, 2> push_constant_range;
    push_constant_range[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    push_constant_range[0].offset = 0;
    push_constant_range[0].size = sizeof(glm::mat4);

    push_constant_range[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    push_constant_range[1].offset = sizeof(glm::mat4);
    push_constant_range[1].size = sizeof(uint32_t);

    //  Pipeline layout
    VkPipelineLayoutCreateInfo pipeline_layout_info{};
    pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipeline_layout_info.setLayoutCount = static_cast<uint32_t>(set_layouts.size());
    pipeline_layout_info.pSetLayouts = set_layouts.data();
    pipeline_layout_info.pushConstantRangeCount = static_cast<uint32_t>(push_constant_range.size());
    pipeline_layout_info.pPushConstantRanges = push_constant_range.data();

    if (vkCreatePipelineLayout(
        device,
        &pipeline_layout_info,
        nullptr,
        &pipeline_layout
    ) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create pipeline layout.");
    }

    VkGraphicsPipelineCreateInfo pipeline_info{};
    pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipeline_info.stageCount = 2;
    pipeline_info.pStages = shader_stages;
    pipeline_info.pVertexInputState = &vertex_input_info;
    pipeline_info.pInputAssemblyState = &input_assembly;
    pipeline_info.pViewportState = &viewport_state;
    pipeline_info.pRasterizationState = &rasterizer;
    pipeline_info.pMultisampleState = &multisampling;
    pipeline_info.pDepthStencilState = &depth_stencil;
    pipeline_info.pColorBlendState = &color_blending;
    pipeline_info.pDynamicState = nullptr; // Optional
    pipeline_info.layout = pipeline_layout;
    pipeline_info.renderPass = render_pass;
    pipeline_info.subpass = 0;
    pipeline_info.basePipelineHandle = VK_NULL_HANDLE; // Optional
    pipeline_info.basePipelineIndex = -1; // Optional

    if (vkCreateGraphicsPipelines(
        device,
        VK_NULL_HANDLE, 1,
        &pipeline_info,
        nullptr,
        &pipeline) != VK_SUCCESS
    ) {
        throw std::runtime_error("Failed to create sprite pipeline.");
    }

    vkDestroyShaderModule(device, frag_shader_module, nullptr);
    vkDestroyShaderModule(device, vert_shader_module, nullptr);
}
}