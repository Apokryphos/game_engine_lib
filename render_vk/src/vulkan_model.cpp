#include "render_vk/mesh.hpp"
#include "render_vk/vulkan.hpp"
#include "render_vk/index_buffer.hpp"
#include "render_vk/vertex_buffer.hpp"
#include "render_vk/vulkan_model.hpp"

namespace render_vk
{
//  ----------------------------------------------------------------------------
void VulkanModel::load(
    VkPhysicalDevice physical_device,
    VkDevice device,
    VulkanQueue& graphics_queue,
    VkCommandPool command_pool,
    std::vector<Mesh>& meshes
) {
    Mesh mesh;
    uint32_t vertex_offset = 0;
    uint32_t index_offset = 0;
    for (const Mesh& m : meshes) {
        mesh.vertices.insert(
            mesh.vertices.end(),
            m.vertices.begin(),
            m.vertices.end()
        );

        mesh.indices.insert(
            mesh.indices.end(),
            m.indices.begin(),
            m.indices.end()
        );

        ModelMesh model_mesh {};
        model_mesh.index_count = m.indices.size();
        model_mesh.vertex_offset = vertex_offset;
        model_mesh.index_offset = index_offset;
        m_meshes.push_back(model_mesh);

        vertex_offset += m.vertices.size();
        index_offset += m.indices.size();
    }

    load(physical_device, device, graphics_queue, command_pool, mesh);
}

//  ----------------------------------------------------------------------------
void VulkanModel::unload() {
    if (m_device != nullptr) {
        vkDestroyBuffer(m_device, m_index_buffer, nullptr);
        vkFreeMemory(m_device, m_index_buffer_memory, nullptr);

        vkDestroyBuffer(m_device, m_vertex_buffer, nullptr);
        vkFreeMemory(m_device, m_vertex_buffer_memory, nullptr);

        m_device = nullptr;
    }
}
}
