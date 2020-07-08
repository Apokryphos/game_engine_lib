#pragma once

#include "render_vk/vulkan.hpp"

namespace render_vk
{
struct VulkanSwapchain;

bool create_graphics_pipeline(
    VkDevice device,
    const VulkanSwapchain& swapchain,
    VkRenderPass render_pass,
    VkDescriptorSetLayout descriptor_set_layout,
    VkPipelineLayout& pipeline_layout,
    VkPipeline& graphics_pipeline
);
}
