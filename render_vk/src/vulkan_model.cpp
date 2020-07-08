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
    VkQueue graphics_queue,
    VkCommandPool command_pool,
    Mesh& mesh
) {
    m_device = device;

    m_index_count = static_cast<uint32_t>(mesh.indices.size());

    create_vertex_buffer(
        physical_device,
        device,
        graphics_queue,
        command_pool,
        m_vertex_buffer,
        m_vertex_buffer_memory
    );

    create_index_buffer(
        physical_device,
        device,
        graphics_queue,
        command_pool,
        m_index_buffer,
        m_index_buffer_memory
    );
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
