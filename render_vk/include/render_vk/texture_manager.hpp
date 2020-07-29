#pragma once

#include "render_vk/texture.hpp"
#include "render_vk/vulkan.hpp"
#include <map>
#include <vector>

namespace render_vk
{
class VulkanQueue;

class TextureManager
{
    typedef uint32_t TextureId;

    VkDevice m_device                  {VK_NULL_HANDLE};
    VkPhysicalDevice m_physical_device {VK_NULL_HANDLE};

    std::map<TextureId, VkFence> m_pending;
    std::vector<Texture> m_textures;

public:
    TextureManager(
        VkPhysicalDevice physical_device,
        VkDevice device
    );
    void destroy_textures();
    const std::vector<Texture>& get_textures() const;
    bool has_pending_textures();
    void load_texture(
        const TextureId texture_id,
        const std::string& path,
        VulkanQueue& queue,
        VkCommandPool command_pool
    );
    bool texture_is_ready(const TextureId texture_id);
};
}
