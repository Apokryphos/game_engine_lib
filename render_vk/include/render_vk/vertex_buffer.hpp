#pragma once

#include "render_vk/vertex.hpp"
#include "render_vk/vulkan.hpp"
#include <vector>

namespace render_vk
{
class VulkanQueue;

void create_vertex_buffer(
    VkPhysicalDevice physical_device,
    VkDevice device,
    VulkanQueue& transfer_queue,
    VkCommandPool command_pool,
    const std::vector<Vertex>& vertices,
    VkBuffer& vertex_buffer,
    VkDeviceMemory& vertex_buffer_memory
);
}
