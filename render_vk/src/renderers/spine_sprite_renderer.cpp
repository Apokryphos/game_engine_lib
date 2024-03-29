#include "render_vk/command_buffer.hpp"
#include "render_vk/command_pool.hpp"
#include "render_vk/debug_utils.hpp"
#include "render_vk/descriptor_set_layout.hpp"
#include "render_vk/renderers/spine_sprite_renderer.hpp"
#include "render_vk/shader.hpp"
#include "render_vk/spine_model.hpp"
#include "render_vk/texture.hpp"
#include "render_vk/vertex.hpp"
#include "render_vk/vulkan_model.hpp"
#include "render_vk/vulkan_spine_manager.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <cassert>
#include <spine/spine.h>
#include <vector>

using namespace render;
using namespace spine;

namespace render_vk
{
static const uint32_t MAX_OBJECTS = 10000;

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
static void build_draw_order(
    const SpineModel& model,
    std::vector<uint32_t>& draw_order
) {
    auto& slots = model.skeleton->getDrawOrder();
    for (uint32_t n = 0; n < slots.size(); ++n) {
        Slot* slot = slots[n];
        SlotData& slot_data = slot->getData();
        uint32_t index = slot_data.getIndex();

        for (const AttachmentInfo& info : model.attachment_infos) {
            if (info.slot == index) {
                draw_order.push_back(info.index);
                break;
            }
        }
    }
}

//  ----------------------------------------------------------------------------
SpineSpriteRenderer::SpineSpriteRenderer(VulkanSpineManager& spine_mgr)
: m_spine_mgr(spine_mgr) {
}

//  ----------------------------------------------------------------------------
void SpineSpriteRenderer::create_objects(
    VkPhysicalDevice physical_device,
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
    const DynamicUniformBuffer<SpineUbo>& uniform_buffer,
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

    uint32_t dynamic_object = 0;

    for (const SpineSpriteBatch& batch : batches) {
        //  Get model
        const SpineModel* spine_model = m_spine_mgr.get_spine_model(batch.spine_id);
        if (spine_model == nullptr) {
            continue;
        }
        const VulkanModel& model = spine_model->model;

        //  Bind vertex buffer
        VkBuffer vertex_buffers[] = { model.get_vertex_buffer() };
        VkDeviceSize vertex_offsets[] = { 0 };

        vkCmdBindVertexBuffers(
            command_buffer,
            0,
            1,
            vertex_buffers,
            vertex_offsets
        );

        //  Bind index buffer
        vkCmdBindIndexBuffer(
            command_buffer,
            model.get_index_buffer(),
            0,
            VK_INDEX_TYPE_UINT32
        );

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
            std::vector<uint32_t> draw_order;
            build_draw_order(*spine_model, draw_order);

            for (const uint32_t attachment_index : draw_order){
                const AttachmentInfo& attachment_info = spine_model->attachment_infos[attachment_index];

                begin_debug_marker(command_buffer, attachment_info.attachment_name.c_str(), DEBUG_MARKER_COLOR_PURPLE);

                float layer = dynamic_object / static_cast<float>(MAX_OBJECTS);

                glm::vec3 position(
                    batch.positions[n].x,
                    batch.positions[n].y,
                    layer
                );

                glm::mat4 model_matrix =
                    glm::translate(glm::mat4(1.0f), position) *
                    glm::scale(glm::mat4(1.0f), batch.sizes[n]);

                vkCmdPushConstants(
                    command_buffer,
                    m_pipeline_layout,
                    VK_SHADER_STAGE_VERTEX_BIT,
                    0,
                    sizeof(glm::mat4),
                    &model_matrix
                );

                const ModelMesh& mesh = model.get_meshes()[attachment_info.index];

                const uint32_t dynamic_align =
                    static_cast<uint32_t>(dynamic_object * uniform_buffer.get_align());

                vkCmdBindDescriptorSets(
                    command_buffer,
                    VK_PIPELINE_BIND_POINT_GRAPHICS,
                    m_pipeline_layout,
                    2,
                    1,
                    &descriptors.spine_set,
                    1,
                    &dynamic_align
                );

                //  Draw
                vkCmdDrawIndexed(
                    command_buffer,
                    mesh.index_count,
                    1,
                    mesh.index_offset,
                    mesh.vertex_offset,
                    1
                );

                end_debug_marker(command_buffer);

                ++dynamic_object;
            }
        }
    }

    if (vkEndCommandBuffer(command_buffer) != VK_SUCCESS) {
        throw std::runtime_error("Failed to record secondary command buffer.");
    }

    end_debug_marker(command_buffer);
}

//  ----------------------------------------------------------------------------
void calculate_transform(
    SpineModel& model,
    std::vector<SpineUbo>& ubos,
    size_t& offset
) {
    // auto skeleton_data = model.skeleton_data;
    // auto& skins = skeleton_data->getSkins();
    // const auto skin_count = skins.size();

    auto skeleton = model.skeleton.get();
    auto& slots = skeleton->getSlots();
    const auto slot_count = slots.size();

    for (size_t n = 0; n < model.attachment_infos.size(); ++n) {
        AttachmentInfo& info = model.attachment_infos[n];

        // Skin* skin = skins[info.skin];
        Slot* slot = slots[info.slot];
        Bone& bone = slot->getBone();

        bone.updateWorldTransform();

        glm::mat4 transform(1.0f);
        transform[0][0] = bone.getA();
        transform[1][0] = bone.getB();
        transform[0][1] = bone.getC();
        transform[1][1] = bone.getD();

        ubos[offset + n].transform = transform;
    }

    offset += model.attachment_infos.size();
}

//  ----------------------------------------------------------------------------
void SpineSpriteRenderer::update_object_uniforms(
    const std::vector<SpineSpriteBatch>& batches,
    DynamicUniformBuffer<SpineUbo>& spine_uniform
) {
    //   Build vectors for uniform buffers
    size_t object_count = 0;
    for (const SpineSpriteBatch& batch : batches) {
        SpineModel* model = m_spine_mgr.get_spine_model(batch.spine_id);
        assert(model != nullptr);
        object_count += (model->attachment_infos.size()) * batch.positions.size();
    }
    assert(object_count > 0);
    assert(object_count < MAX_OBJECTS);

    size_t offset = 0;
    std::vector<SpineUbo> data(object_count);
    for (const SpineSpriteBatch& batch : batches) {
        for (const auto& pos : batch.positions)  {
            SpineModel* model = m_spine_mgr.get_spine_model(batch.spine_id);
            assert(model != nullptr);
            calculate_transform(*model, data, offset);
        }
    }

    //  Copy object UBO structs to dynamic uniform buffer
    spine_uniform.copy(data);
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
    auto vert_shader_code = read_file("assets/shaders/vk/spine_sprite_vert.spv");
    auto frag_shader_code = read_file("assets/shaders/vk/spine_sprite_frag.spv");

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

    std::array<VkDescriptorSetLayout, 3> set_layouts = {
        descriptor_set_layouts.frame,
        descriptor_set_layouts.texture_sampler,
        descriptor_set_layouts.spine,
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
