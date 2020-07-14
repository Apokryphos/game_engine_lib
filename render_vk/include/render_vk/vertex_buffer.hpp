#pragma once

#include "render_vk/vertex.hpp"
#include "render_vk/vulkan.hpp"
#include <vector>

namespace render_vk
{
void create_vertex_buffer(
    VkPhysicalDevice physical_device,
    VkDevice device,
    VkQueue transfer_queue,
    VkCommandPool command_pool,
    const std::vector<Vertex>& vertices,
    VkBuffer& vertex_buffer,
    VkDeviceMemory& vertex_buffer_memory
);
}
