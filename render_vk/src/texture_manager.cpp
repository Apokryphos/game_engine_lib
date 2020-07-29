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
bool TextureManager::has_pending_textures() {
    //  TODO: Use fences
    if (!m_pending.empty()) {
        vkDeviceWaitIdle(m_device);
        m_pending.clear();
    }

    return false;

    // return !m_pending.empty();
}

//  ----------------------------------------------------------------------------
void TextureManager::load_texture(
    const TextureId texture_id,
    const std::string& path,
    VulkanQueue& queue,
    VkCommandPool command_pool
) {
    //  TODO: Create actual fence
    VkFence fence = VK_NULL_HANDLE;
    m_pending[texture_id] = fence;

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
}

//  ----------------------------------------------------------------------------
bool TextureManager::texture_is_ready(const TextureId texture_id) {
    const auto& find = m_pending.find(texture_id);

    if (find == m_pending.end()) {
        //  Transfer is not in pending queue.
        return true;
    }

    VkFence fence = find->second;
    const VkResult result = vkGetFenceStatus(m_device, fence);

    if (result == VK_SUCCESS) {
        //  Transfer is complete.
        vkDestroyFence(m_device, fence, nullptr);
        //  Remove from pending queue.
        m_pending.erase(find);
        return true;
    }

    if (result == VK_NOT_READY) {
        //  Transfer is not finished.
        return false;
    }

    throw std::runtime_error("Device lost.");
}
}
