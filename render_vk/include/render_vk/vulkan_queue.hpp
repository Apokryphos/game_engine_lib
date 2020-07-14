#pragma once

#include "render_vk/buffer.hpp"
#include "render_vk/vulkan.hpp"
#include <mutex>
#include <vector>

namespace render_vk
{
class VulkanQueue
{
    VkDevice m_device;
    VkPhysicalDevice m_physical_device;

    std::mutex m_queue_mutex;
    VkQueue m_queue;

public:
    VulkanQueue()
    : m_physical_device(VK_NULL_HANDLE),
      m_device(VK_NULL_HANDLE),
      m_queue(VK_NULL_HANDLE) {
    }

    VulkanQueue(const VulkanQueue&) = delete;
    VulkanQueue& operator=(const VulkanQueue&) = delete;

    //  Copies a buffer to the GPU
    template <typename T>
    void copy_buffer(
        VkCommandPool command_pool,
        VkBuffer buffer,
        const std::vector<T>& data
    ) {
        void* staging_data;
        VkBuffer staging_buffer;
        VkDeviceMemory staging_buffer_memory;
        VkDeviceSize buffer_size = sizeof(T) * data.size();

        //  Create staging buffer
        render_vk::create_buffer(
            m_physical_device,
            m_device,
            buffer_size,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            staging_buffer,
            staging_buffer_memory
        );

        //  Copy data to staging buffer
        vkMapMemory(m_device, staging_buffer_memory, 0, buffer_size, 0, &staging_data);
        memcpy(staging_data, data.data(), (size_t)buffer_size);
        vkUnmapMemory(m_device, staging_buffer_memory);

        //  Copy staging buffer to destination buffer
        std::lock_guard<std::mutex> lock(m_queue_mutex);
        copy_buffer(
            command_pool,
            staging_buffer,
            buffer,
            buffer_size
        );

        //  Free staging buffer
        vkDestroyBuffer(m_device, staging_buffer, nullptr);
        vkFreeMemory(m_device, staging_buffer_memory, nullptr);
    }

    //  Copies between two buffers
    void copy_buffer(
        VkCommandPool command_pool,
        VkBuffer src,
        VkBuffer dst,
        VkDeviceSize size
    );

    void end_single_time_commands(
        VkCommandPool command_pool,
        VkCommandBuffer command_buffer
    );

    VkQueue get_queue() {
        return m_queue;
    }

    void initialize(
        VkPhysicalDevice physical_device,
        VkDevice device,
        VkQueue queue
    ) {
        m_physical_device = physical_device;
        m_device = device;
        m_queue = queue;
    }

    VkResult submit(
        uint32_t submit_info_count,
        const VkSubmitInfo& submit_info,
        VkFence fence = VK_NULL_HANDLE
    );
};
}
