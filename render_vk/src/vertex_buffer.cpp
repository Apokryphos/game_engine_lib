#include "render_vk/buffer.hpp"
#include "render_vk/vertex.hpp"
#include "render_vk/vulkan.hpp"
#include "render_vk/vulkan_queue.hpp"
#include <cstring>
#include <stdexcept>
#include <vector>

namespace render_vk
{
//  ----------------------------------------------------------------------------
void create_vertex_buffer(
    VkPhysicalDevice physical_device,
    VkDevice device,
    VulkanQueue& transfer_queue,
    VkCommandPool command_pool,
    const std::vector<Vertex>& vertices,
    VkBuffer& vertex_buffer,
    VkDeviceMemory& vertex_buffer_memory
) {
    //  Create vertex buffer
    VkDeviceSize buffer_size = sizeof(Vertex) * vertices.size();

    VkBuffer staging_buffer;
    VkDeviceMemory staging_buffer_memory;

    create_buffer(
        physical_device,
        device,
        buffer_size,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        staging_buffer,
        staging_buffer_memory
    );

    void* data;
    vkMapMemory(device, staging_buffer_memory, 0, buffer_size, 0, &data);
    memcpy(data, vertices.data(), (size_t)buffer_size);
    vkUnmapMemory(device, staging_buffer_memory);

    create_buffer(
        physical_device,
        device,
        buffer_size,
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        vertex_buffer,
        vertex_buffer_memory
    );

    transfer_queue.copy_buffer(
        command_pool,
        staging_buffer,
        vertex_buffer,
        buffer_size
    );

    //  Free staging buffer
    vkDestroyBuffer(device, staging_buffer, nullptr);
    vkFreeMemory(device, staging_buffer_memory, nullptr);
}
}
