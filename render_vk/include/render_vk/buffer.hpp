#pragma once

#include "render_vk/vulkan.hpp"
#include <cstdint>

namespace render_vk
{
void create_buffer(
    VkPhysicalDevice physical_device,
    VkDevice device,
    VkDeviceSize size,
    VkBufferUsageFlags usage,
    VkMemoryPropertyFlags properties,
    VkBuffer& buffer,
    VkDeviceMemory& buffer_memory
);

void copy_buffer(
    VkDevice device,
    VkQueue transfer_queue,
    VkCommandPool command_pool,
    VkBuffer src,
    VkBuffer dst,
    VkDeviceSize size
);

uint32_t find_memory_type(
    VkPhysicalDevice physical_device,
    uint32_t type_filter,
    VkMemoryPropertyFlags properties
);
}
