#pragma once

#include "assets/asset_id.hpp"
#include "render_vk/index_buffer.hpp"
#include "render_vk/mesh.hpp"
#include "render_vk/vertex_buffer.hpp"
#include "render_vk/vulkan.hpp"
#include <vector>

namespace render_vk
{
struct GlyphMesh;
class VulkanQueue;

struct ModelMesh
{
    uint32_t index_count;
    uint32_t index_offset;
    uint32_t vertex_offset;
};

class VulkanModel
{
    using AssetId = assets::AssetId;

    AssetId m_id;
    uint32_t m_index_count;
    VkDevice m_device;
    VkBuffer m_vertex_buffer;
    VkDeviceMemory m_vertex_buffer_memory;
    VkBuffer m_index_buffer;
    VkDeviceMemory m_index_buffer_memory;
    std::vector<ModelMesh> m_meshes;

public:
    VulkanModel(const assets::AssetId id = 0)
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

    inline const std::vector<ModelMesh>& get_meshes() const {
        return m_meshes;
    }

    inline VkBuffer get_vertex_buffer() const {
        return m_vertex_buffer;
    }

    template <typename T>
    void load(
        VkPhysicalDevice physical_device,
        VkDevice device,
        VulkanQueue& graphics_queue,
        VkCommandPool command_pool,
        MeshBase<T>& mesh
    ) {
        m_device = device;

        m_index_count = static_cast<uint32_t>(mesh.indices.size());

        create_vertex_buffer(
            physical_device,
            device,
            graphics_queue,
            command_pool,
            mesh.vertices,
            m_vertex_buffer,
            m_vertex_buffer_memory
        );

        create_index_buffer(
            physical_device,
            device,
            graphics_queue,
            command_pool,
            mesh.indices,
            m_index_buffer,
            m_index_buffer_memory
        );
    }

    void load(
        VkPhysicalDevice physical_device,
        VkDevice device,
        VulkanQueue& graphics_queue,
        VkCommandPool command_pool,
        std::vector<Mesh>& meshes
    );

    void unload();
};
}
