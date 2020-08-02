#pragma once

#include "render_vk/vulkan.hpp"

namespace render_vk
{
struct VulkanSwapchain;

//  Multisampled color buffer for MSAA
struct ColorImage
{
    VkImage image         {VK_NULL_HANDLE};
    VkDeviceMemory memory {VK_NULL_HANDLE};
    VkImageView view      {VK_NULL_HANDLE};
};

void create_color_resources(
    VkPhysicalDevice physical_device,
    VkDevice device,
    const VulkanSwapchain& swapchain,
    VkSampleCountFlagBits msaa_sample_count,
    ColorImage& color_image
);
}
