#pragma once

#include "render_vk/descriptor_set_layout.hpp"
#include "render_vk/vulkan.hpp"

namespace render_vk
{
struct VulkanSwapchain;

void create_graphics_pipeline(
    VkDevice device,
    const VulkanSwapchain& swapchain,
    VkRenderPass render_pass,
    const DescriptorSetLayouts& descriptor_set_layouts,
    // VkDescriptorSetLayout descriptor_set_layout,
    VkPipelineLayout& pipeline_layout,
    VkPipeline& graphics_pipeline
);
}
