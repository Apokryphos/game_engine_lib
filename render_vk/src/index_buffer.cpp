#include "render_vk/buffer.hpp"
#include "render_vk/vulkan.hpp"
#include <cstdint>
#include <cstring>
#include <vector>

namespace render_vk
{
//  ----------------------------------------------------------------------------
void create_index_buffer(
    VkPhysicalDevice physical_device,
    VkDevice device,
    VkQueue transfer_queue,
    VkCommandPool command_pool,
    const std::vector<uint32_t>& indices,
    VkBuffer& index_buffer,
    VkDeviceMemory& index_buffer_memory
) {
    //  Create vertex buffer
    VkDeviceSize buffer_size = sizeof(uint32_t) * indices.size();

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
    memcpy(data, indices.data(), (size_t)buffer_size);
    vkUnmapMemory(device, staging_buffer_memory);

    create_buffer(
        physical_device,
        device,
        buffer_size,
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        index_buffer,
        index_buffer_memory
    );

    copy_buffer(
        device,
        transfer_queue,
        command_pool,
        staging_buffer,
        index_buffer,
        buffer_size
    );

    //  Free staging buffer
    vkDestroyBuffer(device, staging_buffer, nullptr);
    vkFreeMemory(device, staging_buffer_memory, nullptr);
}
}
