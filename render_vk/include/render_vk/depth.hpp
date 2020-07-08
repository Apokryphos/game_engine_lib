#pragma once

#include "render_vk/vulkan.hpp"

namespace render_vk
{
struct VulkanSwapchain;

VkFormat find_depth_format(VkPhysicalDevice physical_device);
bool has_stencil_component(VkFormat format);

void create_depth_resources(
    VkPhysicalDevice physical_device,
    VkDevice device,
    VkQueue transfer_queue,
    VkCommandPool command_pool,
    const VulkanSwapchain& swapchain,
    VkImage& depth_image,
    VkImageView& depth_image_view,
    VkDeviceMemory& depth_image_memory
);
}
