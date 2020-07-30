#include "common/log.hpp"
#include "render_vk/texture_manager.hpp"
#include "render_vk/vulkan_queue.hpp"
#include <algorithm>

using namespace common;

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
void TextureManager::add_texture(const Texture& texture) {
    std::lock_guard<std::mutex> lock(m_mutex);

    m_added.push_back(texture);

    log_debug("Texture %d completed transfer.", texture.id);
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
    std::lock_guard<std::mutex> lock(m_mutex);

    Texture texture{};
    create_texture(
        m_physical_device,
        m_device,
        queue,
        command_pool,
        path,
        texture
    );

    texture.id = texture_id;

    m_textures.push_back(texture);

    ++m_timestamp;
}

//  ----------------------------------------------------------------------------
bool TextureManager::texture_exists(const TextureId texture_id) {
    std::lock_guard<std::mutex> lock(m_mutex);

    return std::find_if(
        m_textures.begin(),
        m_textures.end(),
        [texture_id](const Texture& texture) {
            return texture.id == texture_id;
        }
    ) != m_textures.end();
}

//  ----------------------------------------------------------------------------
void TextureManager::update_textures() {
    std::lock_guard<std::mutex> lock(m_mutex);

    if (m_added.empty()) {
        return;
    }

    m_textures.insert(m_textures.end(), m_added.begin(), m_added.end());
    m_added.clear();

    ++m_timestamp;
}
}
