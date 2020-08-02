#pragma once

#include "render_vk/vulkan.hpp"

namespace render_vk
{
class VulkanQueue;
struct VulkanSwapchain;

struct DepthImage
{
    VkImage image = VK_NULL_HANDLE;
    VkDeviceMemory memory = VK_NULL_HANDLE;
    VkImageView view = VK_NULL_HANDLE;
};

VkFormat find_depth_format(VkPhysicalDevice physical_device);
bool has_stencil_component(VkFormat format);

void create_depth_resources(
    VkPhysicalDevice physical_device,
    VkDevice device,
    VulkanQueue& transfer_queue,
    VkCommandPool command_pool,
    const VulkanSwapchain& swapchain,
    VkSampleCountFlagBits msaa_sample_count,
    VkImage& depth_image,
    VkImageView& depth_image_view,
    VkDeviceMemory& depth_image_memory
);

void create_depth_resources(
    VkPhysicalDevice physical_device,
    VkDevice device,
    VulkanQueue& transfer_queue,
    VkCommandPool command_pool,
    const VulkanSwapchain& swapchain,
    VkSampleCountFlagBits msaa_sample_count,
    DepthImage& depth_image
);
}
