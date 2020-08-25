#include "render_vk/command_buffer.hpp"
#include "render_vk/command_pool.hpp"
#include "render_vk/debug_utils.hpp"
#include "render_vk/descriptor_set_layout.hpp"
#include "render_vk/model_manager.hpp"
#include "render_vk/renderers/glyph_renderer.hpp"
#include "render_vk/shader.hpp"
#include "render_vk/vertex.hpp"
#include "render_vk/vulkan_model.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

using namespace render;

namespace render_vk
{
//  ----------------------------------------------------------------------------
static void create_glyph_pipeline(
    VkDevice device,
    const VulkanSwapchain& swapchain,
    VkRenderPass render_pass,
    const VkSampleCountFlagBits msaa_sample_count,
    const DescriptorSetLayouts& descriptor_set_layouts,
    VkPipelineLayout& pipeline_layout,
    VkPipeline& pipeline
);

//  ----------------------------------------------------------------------------
GlyphRenderer::GlyphRenderer(ModelManager& model_mgr)
: m_model_mgr(model_mgr) {
}

//  ----------------------------------------------------------------------------
void GlyphRenderer::create_objects(
    VkDevice device,
    const VulkanSwapchain& swapchain,
    VkRenderPass render_pass,
    const VkSampleCountFlagBits msaa_sample_count,
    const DescriptorSetLayouts& descriptor_set_layouts
) {
    m_device = device;
    m_render_pass = render_pass;

    create_glyph_pipeline(
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
void GlyphRenderer::destroy_objects() {
    vkDestroyPipeline(m_device, m_pipeline, nullptr);
    m_pipeline = VK_NULL_HANDLE;

    vkDestroyPipelineLayout(m_device, m_pipeline_layout, nullptr);
    m_pipeline_layout = VK_NULL_HANDLE;

    m_render_pass = VK_NULL_HANDLE;
    m_device = VK_NULL_HANDLE;
}

//  ----------------------------------------------------------------------------
void GlyphRenderer::draw_glyphs(
    const uint32_t instance_count,
    const FrameDescriptorObjects& descriptors,
    const FrameUniformObjects& uniform_buffers,
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

    begin_debug_marker(command_buffer, "Draw Glyphs", DEBUG_MARKER_COLOR_ORANGE);
    if (vkBeginCommandBuffer(command_buffer, &begin_info) != VK_SUCCESS) {
        throw std::runtime_error("Failed to begin recording command buffer.");
    }

    //  Check if batches are empty
    if (instance_count == 0) {
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

    //  Bind descriptors
    std::array<VkDescriptorSet, 3> descriptor_sets {
        descriptors.frame_set,
        descriptors.texture_set,
        descriptors.glyph_set,
    };
    std::array<uint32_t, 1> dynamic_offsets {0};
    vkCmdBindDescriptorSets(
        command_buffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        m_pipeline_layout,
        0,
        static_cast<uint32_t>(descriptor_sets.size()),
        descriptor_sets.data(),
        dynamic_offsets.size(),
        dynamic_offsets.data()
    );

    //  Get sprite quad
    const VulkanModel& quad = m_model_mgr.get_sprite_quad();
    const uint32_t index_count = quad.get_index_count();

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

    //  Draw instances
    vkCmdDrawIndexed(command_buffer, index_count, instance_count, 0, 0, 0);

    if (vkEndCommandBuffer(command_buffer) != VK_SUCCESS) {
        throw std::runtime_error("Failed to record secondary command buffer.");
    }

    end_debug_marker(command_buffer);
}

//  ----------------------------------------------------------------------------
static void create_glyph_pipeline(
    VkDevice device,
    const VulkanSwapchain& swapchain,
    VkRenderPass render_pass,
    const VkSampleCountFlagBits msaa_sample_count,
    const DescriptorSetLayouts& descriptor_set_layouts,
    VkPipelineLayout& pipeline_layout,
    VkPipeline& pipeline
) {
    //  Shaders
    auto vert_shader_code = read_file("assets/shaders/vk/glyph_vert.spv");
    auto frag_shader_code = read_file("assets/shaders/vk/glyph_frag.spv");

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
        descriptor_set_layouts.glyph,
    };

    //  Pipeline layout
    VkPipelineLayoutCreateInfo pipeline_layout_info{};
    pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipeline_layout_info.setLayoutCount = static_cast<uint32_t>(set_layouts.size());
    pipeline_layout_info.pSetLayouts = set_layouts.data();
    pipeline_layout_info.pushConstantRangeCount = 0;
    pipeline_layout_info.pPushConstantRanges = nullptr;

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
