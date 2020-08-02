#pragma once

#include "render_vk/vulkan.hpp"

namespace render_vk
{
class VulkanQueue;

void create_image(
    VkPhysicalDevice physical_device,
    VkDevice device,
    uint32_t width,
    uint32_t height,
    uint32_t mip_levels,
    VkSampleCountFlagBits sample_count,
    VkFormat format,
    VkImageTiling tiling,
    VkImageUsageFlags usage,
    VkMemoryPropertyFlags properties,
    VkImage& image,
    VkDeviceMemory& image_memory
);

//  Records transition image layout commands to a command buffer.
void record_transition_image_layout_commands(
    VkCommandBuffer command_buffer,
    VkImage image,
    VkFormat format,
    uint32_t mip_levels,
    VkImageLayout old_layout,
    VkImageLayout new_layout
);

//  Submits transition image layout commands to a queue using single-time
//  commands.
void transition_image_layout(
    VkDevice device,
    VulkanQueue& transfer_queue,
    VkCommandPool command_pool,
    VkImage image,
    VkFormat format,
    uint32_t mip_levels,
    VkImageLayout old_layout,
    VkImageLayout new_layout
);
}
