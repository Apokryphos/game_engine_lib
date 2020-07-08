#pragma once

#include "render_vk/vulkan.hpp"
#include <cstdint>
#include <vector>

namespace render_vk
{
void create_index_buffer(
    VkPhysicalDevice physical_device,
    VkDevice device,
    VkQueue transfer_queue,
    VkCommandPool command_pool,
    VkBuffer& index_buffer,
    VkDeviceMemory& index_buffer_memory
);

std::vector<uint32_t>& get_indices();
}
