#pragma once

#include "render_vk/texture.hpp"
#include "render_vk/vulkan.hpp"
#include <map>
#include <mutex>
#include <vector>

namespace render_vk
{
class VulkanQueue;

class TextureManager
{
    //  Changes when textures are added or removed.
    uint32_t m_timestamp {1};

    mutable std::mutex m_mutex;

    VkDevice m_device                  {VK_NULL_HANDLE};
    VkPhysicalDevice m_physical_device {VK_NULL_HANDLE};

    std::vector<Texture> m_textures;
    std::vector<Texture> m_added;

public:
    TextureManager(
        VkPhysicalDevice physical_device,
        VkDevice device
    );
    TextureManager(const TextureManager&) = delete;
    TextureManager& operator=(const TextureManager&) = delete;
    void add_texture(const Texture& texture);
    void destroy_textures();
    void get_textures(std::vector<Texture>& textures);

    //  Returns timestamp used to determine if textures were added or removed
    inline uint32_t get_timestamp() const {
        std::lock_guard<std::mutex> lock(m_mutex);

        return m_timestamp;
    }

    //  Loads a texture and waits for the queue to finish execution
    void load_texture(
        const TextureId texture_id,
        const std::string& path,
        VulkanQueue& queue,
        VkCommandPool command_pool
    );

    bool texture_exists(const TextureId texture_id) const;
    void update_textures();
};
}
