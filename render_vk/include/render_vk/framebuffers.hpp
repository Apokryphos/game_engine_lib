#pragma once

#include "render_vk/vulkan.hpp"

namespace render_vk
{
struct VulkanSwapchain;

bool create_framebuffers(
    VkDevice device,
    VkRenderPass render_pass,
    VulkanSwapchain& swapchain
);

bool create_framebuffers(
    VkDevice device,
    VkRenderPass render_pass,
    VkImageView depth_image_view,
    VulkanSwapchain& swapchain
);
}
