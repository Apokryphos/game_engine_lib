#pragma once

#include "render_vk/vulkan_render_system.hpp"
#include "render_vk/vulkan.hpp"

namespace render_vk
{
void task_draw_models(
    const VkRenderPass render_pass,
    const VkPipelineLayout pipeline_layout,
    const VkPipeline graphics_pipeline,
    ModelManager& model_mgr,
    const VulkanRenderSystem::FrameDescriptorObjects& descriptors,
    const std::vector<render::ModelBatch>& batches,
    VkCommandBuffer command_buffer
);
}
