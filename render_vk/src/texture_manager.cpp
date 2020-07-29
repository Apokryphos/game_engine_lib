#include "render_vk/texture_manager.hpp"
#include "render_vk/vulkan_queue.hpp"

namespace render_vk
{
//  ----------------------------------------------------------------------------
TextureManager::TextureManager(
    VkPhysicalDevice physical_device,
    VkDevice device
)
: m_physical_device(physical_device),
  m_device(device) {
    m_textures.reserve(MAX_TEXTURES);
}

//  ----------------------------------------------------------------------------
void TextureManager::add_texture(const TextureId texture_id, const Texture& texture) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_textures.push_back(texture);
    ++m_timestamp;
}

//  ----------------------------------------------------------------------------
void TextureManager::destroy_textures() {
    for (Texture& texture : m_textures) {
        destroy_texture(m_device, texture);
    }

    m_textures.clear();
}

//  ----------------------------------------------------------------------------
void TextureManager::get_textures(std::vector<Texture>& textures) {
    std::lock_guard<std::mutex> lock(m_mutex);
    textures = m_textures;
}

//  ----------------------------------------------------------------------------
void TextureManager::load_texture(
    const TextureId texture_id,
    const std::string& path,
    VulkanQueue& queue,
    VkCommandPool command_pool
) {
    Texture texture{};
    create_texture(
        m_physical_device,
        m_device,
        queue,
        command_pool,
        path,
        texture
    );

    m_textures.push_back(texture);

    ++m_timestamp;
}
}
