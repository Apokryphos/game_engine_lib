#pragma once

#include "render_vk/vulkan.hpp"

namespace render_vk
{
struct VulkanSwapchain;

bool create_render_pass(
    VkDevice device,
    VkPhysicalDevice physical_device,
    const VulkanSwapchain& swapchain,
    const VkSampleCountFlagBits msaa_sample_count,
    VkRenderPass& render_pass
);
}
