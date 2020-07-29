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
void TextureManager::destroy_textures() {
    for (Texture& texture : m_textures) {
        destroy_texture(m_device, texture);
    }

    m_textures.clear();
}

//  ----------------------------------------------------------------------------
const std::vector<Texture>& TextureManager::get_textures() const {
    return m_textures;
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
