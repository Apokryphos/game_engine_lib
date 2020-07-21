#pragma once

#include "render_vk/vulkan.hpp"
#include <glm/mat4x4.hpp>
#include <vector>

namespace render_vk
{
struct DescriptorSetLayouts
{
    //  Per-frame (view, projection)
    VkDescriptorSetLayout frame = VK_NULL_HANDLE;

    //  Per-object (model, texture)
    // VkDescriptorSetLayout object = VK_NULL_HANDLE;

    //  Textures
    VkDescriptorSetLayout texture_sampler = VK_NULL_HANDLE;
};

void create_descriptor_set_layouts(
    const VkDevice device,
    const uint32_t sampler_count,
    DescriptorSetLayouts& layouts
);
}
