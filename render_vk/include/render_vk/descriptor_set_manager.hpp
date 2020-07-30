#pragma once

#include "render_vk/texture.hpp"
#include "render_vk/vulkan.hpp"
#include <mutex>
#include <vector>

namespace render_vk
{
class TextureManager;

//  Manages descriptor set templates that can be copied to thread created
//  descriptor sets.
class DescriptorSetManager
{
    uint32_t m_texture_timestamp {0};
    mutable std::mutex m_mutex;
    VkDevice m_device {VK_NULL_HANDLE};
    std::vector<Texture> m_textures;
    TextureManager& m_texture_mgr;

public:
    DescriptorSetManager(VkDevice device, TextureManager& texture_mgr);
    void copy_texture_descriptor_set(VkDescriptorSet dst);
    bool is_ready() const;
    void update_descriptor_sets(TextureManager& texture_mgr);
};
}
