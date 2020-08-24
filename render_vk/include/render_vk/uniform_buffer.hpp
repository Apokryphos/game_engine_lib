#pragma once

#include "common/alloc.hpp"
#include "render_vk/buffer.hpp"
#include "render_vk/vulkan.hpp"
#include <cassert>
#include <string.h>
#include <vector>

namespace render_vk
{
//  Uniform buffer.
//  Stores a single UBO (of type T).
template <typename T>
class UniformBuffer
{
    VkDevice m_device {VK_NULL_HANDLE};

    //  Uniform buffer
    VkBuffer m_buffer {VK_NULL_HANDLE};

    //  Uniform buffer memory
    VkDeviceMemory m_memory {VK_NULL_HANDLE};

    //  Mapped memory for UBO data (CPU -> GPU)
    void* m_mapped {nullptr};

public:
    UniformBuffer() = default;
    UniformBuffer(const UniformBuffer&) = delete;
    UniformBuffer& operator=(const UniformBuffer&) = delete;

    //  Copies UBO struct to mapped memory
    void copy(const T& ubo) {
        memcpy(m_mapped, &ubo, sizeof(T));
    }

    void create(VkPhysicalDevice physical_device, VkDevice device) {
        m_device = device;

        //  Create UBO uniform buffer (GPU)
        create_buffer(
            physical_device,
            device,
            sizeof(T),
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            m_buffer,
            m_memory
        );

        //  Map UBO data
        assert(m_memory != VK_NULL_HANDLE);
        assert(m_mapped == nullptr);
        vkMapMemory(
            device,
            m_memory,
            0,
            sizeof(T),
            0,
            &m_mapped
        );
        assert(m_mapped != nullptr);
    }

    void destroy() {
        //  Unmap UBO data
        if (m_mapped != VK_NULL_HANDLE) {
            vkUnmapMemory(m_device, m_memory);
            m_mapped = VK_NULL_HANDLE;
        }

        //  Destroy uniform buffer
        vkDestroyBuffer(m_device, m_buffer, nullptr);
        m_buffer = VK_NULL_HANDLE;

        //  Free uniform buffer memory
        vkFreeMemory(m_device, m_memory, nullptr);
        m_memory = VK_NULL_HANDLE;

        //  Clear device
        m_device = VK_NULL_HANDLE;
    }

    VkBuffer get_buffer() const {
        return m_buffer;
    }

    size_t get_ubo_size() const {
        return sizeof(T);
    }
};
}
