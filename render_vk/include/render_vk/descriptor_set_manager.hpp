#pragma once

#include "render_vk/vulkan.hpp"
#include <mutex>

namespace render_vk
{
class TextureManager;

//  Manages descriptor set templates that can be copied to thread created
//  descriptor sets.
class DescriptorSetManager
{
    bool m_is_ready {false};
    uint32_t m_texture_timestamp {0};
    uint32_t m_texture_descriptors_count {0};
    mutable std::mutex m_mutex;
    VkDevice m_device {VK_NULL_HANDLE};
    VkDescriptorPool m_descriptor_pool {VK_NULL_HANDLE};
    VkDescriptorSet m_texture_descriptor_set {VK_NULL_HANDLE};

public:
    DescriptorSetManager(
        VkDevice device,
        VkDescriptorSetLayout texture_descriptor_set_layout
    );
    void copy_texture_descriptor_set(VkDescriptorSet dst);
    void destroy();
    bool is_ready() const;
    void update_descriptor_sets(TextureManager& texture_mgr);
};
}
