#pragma once

#include "common/asset.hpp"

namespace render_vk
{
struct Mesh;
class VulkanQueue;

class VulkanModel
{
    using AssetId = common::AssetId;

    AssetId m_id;
    uint32_t m_index_count;
    VkDevice m_device;
    VkBuffer m_vertex_buffer;
    VkDeviceMemory m_vertex_buffer_memory;
    VkBuffer m_index_buffer;
    VkDeviceMemory m_index_buffer_memory;

public:
    VulkanModel(const common::AssetId id)
    : m_id(id),
      m_device(nullptr) {
    }

    virtual ~VulkanModel() {
        unload();
    }

    inline AssetId get_id() const {
        return m_id;
    }

    inline VkBuffer get_index_buffer() const {
        return m_index_buffer;
    }

    inline uint32_t get_index_count() const {
        return m_index_count;
    }

    inline VkBuffer get_vertex_buffer() const {
        return m_vertex_buffer;
    }

    void load(
        VkPhysicalDevice physical_device,
        VkDevice device,
        VulkanQueue& graphics_queue,
        VkCommandPool command_pool,
        Mesh& mesh
    );

    void unload();
};
}
