#pragma once

#include "common/alloc.hpp"
#include "render_vk/buffer.hpp"
#include "render_vk/vulkan.hpp"
#include <cassert>
#include <string.h>
#include <vector>

namespace render_vk
{
void get_dynamic_buffer_align_and_size(
    VkPhysicalDevice physical_device,
    const size_t object_count,
    const size_t uniform_buffer_size,
    size_t& dynamic_align,
    size_t& buffer_size
);

//  Dynamic uniform buffer.
//  Stores UBO's (of type T).
template <typename T>
class DynamicUniformBuffer
{
    //  Maximum number of object instances this buffer will support
    uint32_t m_object_count {0};

    //  Dynamic buffer alignment (bytes)
    size_t m_align {0};

    //  Dynamic buffer size (bytes)
    size_t m_buffer_size {0};

    VkDevice m_device {VK_NULL_HANDLE};

    //  Dynamic uniform buffer
    VkBuffer m_buffer {VK_NULL_HANDLE};

    //  Dynamic uniform buffer memory
    VkDeviceMemory m_memory {VK_NULL_HANDLE};

    //  Mapped memory for UBO data (CPU -> GPU)
    void* m_mapped {nullptr};

    //  Storage for UBO structs
    T* m_ubo_data {nullptr};

public:
    DynamicUniformBuffer() = default;
    DynamicUniformBuffer(const DynamicUniformBuffer&) = delete;
    DynamicUniformBuffer& operator=(const DynamicUniformBuffer&) = delete;

    //  Copies UBO structs to mapped memory
    void copy(const std::vector<T>& ubos) {
        assert(ubos.size() <= m_object_count);

        //  Copy structs to aligned memory
        char* ubo_char = (char*)m_ubo_data;
        for (size_t n = 0; n < ubos.size(); ++n) {
            const T& ubo = ubos[n];

            memcpy(
                &ubo_char[n * m_align],
                &ubo,
                sizeof(T)
            );
        }

        //  Copy aligned UBO data to uniform buffer mapped memory
        memcpy(
            m_mapped,
            m_ubo_data,
            m_buffer_size
        );
    }

    void create(
        VkPhysicalDevice physical_device,
        VkDevice device,
        uint32_t object_count
    ) {
        m_object_count = object_count;
        assert(m_object_count > 0);

        m_device = device;

        //  Determine alignment and calculate buffer size
        get_dynamic_buffer_align_and_size(
            physical_device,
            m_object_count,
            sizeof(T),
            m_align,
            m_buffer_size
        );

        //  Allocate data buffer (where UBO data will be stored in RAM)
        assert(m_ubo_data == nullptr);
        m_ubo_data = (T*)common::aligned_alloc(
            m_buffer_size,
            m_align
        );
        assert(m_ubo_data != nullptr);

        //  Create UBO dynamic uniform buffer (GPU)
        create_buffer(
            physical_device,
            device,
            m_buffer_size,
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
            m_buffer_size,
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

        //  Free UBO data
        if (m_ubo_data != nullptr) {
            free(m_ubo_data);
            m_ubo_data = nullptr;
        }

        //  Destroy dynamic uniform buffer
        vkDestroyBuffer(m_device, m_buffer, nullptr);
        m_buffer = VK_NULL_HANDLE;

        //  Free dynamic uniform buffer memory
        vkFreeMemory(m_device, m_memory, nullptr);
        m_memory = VK_NULL_HANDLE;

        //  Clear device
        m_device = VK_NULL_HANDLE;
    }

    size_t get_align() const {
        return m_align;
    }

    VkBuffer get_buffer() const {
        return m_buffer;
    }
};

//  ----------------------------------------------------------------------------
void get_dynamic_buffer_align_and_size(
    VkPhysicalDevice physical_device,
    const size_t object_count,
    const size_t uniform_buffer_size,
    size_t& dynamic_align,
    size_t& buffer_size
);
}
