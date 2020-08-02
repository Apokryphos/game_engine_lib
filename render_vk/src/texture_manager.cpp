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
}

//  ----------------------------------------------------------------------------
void TextureManager::destroy_textures() {
    for (Texture& texture : m_textures) {
        destroy_texture(m_device, texture);
    }
    m_textures.clear();

    for (Texture& texture : m_added) {
        destroy_texture(m_device, texture);
    }
    m_added.clear();
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

    texture.id = texture_id;

    add_texture(texture);

    log_debug("Loaded texture '%s' (%d).", path.c_str(), texture_id);
}

//  ----------------------------------------------------------------------------
bool TextureManager::texture_exists(const TextureId texture_id) const {
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

    //  Texture vector needs to match shader texture array.
    //  Textures need to be in correct order matching texture IDs.
    //  Any missing textures should be filled in with a placeholder
    //  texture so they can be identified and to prevent validation
    //  layer errors.

    //  Get last ID of new textures
    const uint32_t last_id = std::max_element(
        m_added.begin(),
        m_added.end(),
        [](const Texture& a, const Texture& b){
            return a.id < b.id;
        }
    )->id;

    //  Check if textures vector needs to be resized
    const size_t count = m_textures.size();
    if (last_id >= count) {
        //  Resize textures vector
        m_textures.resize(last_id + 1);
        const size_t new_count = m_textures.size();

        //  TODO: Texture for missing textures should be loaded separately
        //  For now, use the first available texture.
        const Texture empty_texture =
            count == 0 ?
            m_added.at(0) :
            m_textures.at(0);

        //  Fill new slots with copy of empty texture
        for (size_t n = count; n < new_count; ++n) {
            m_textures[n] = empty_texture;
            m_textures[n].id = n;
        }
    }

    //  Add new textures
    for (const Texture& texture : m_added) {
        m_textures.at(texture.id) = texture;
    }

    //  Clear added textures
    m_added.clear();

    //  Sort textures by ID
    // std::sort(
    //     m_textures.begin(),
    //     m_textures.end(),
    //     [](const Texture& a, const Texture& b) {
    //         return a.id < b.id;
    //     }
    // );

    ++m_timestamp;

    log_debug("Textures updated.");
}
}
