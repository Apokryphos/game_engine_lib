#pragma once

#include "render_vk/vulkan.hpp"
#include <glm/mat4x4.hpp>
#include <vector>

namespace render_vk
{
struct DescriptorSetLayouts
{
    //  Per-frame (view, projection)
    VkDescriptorSetLayout frame;

    //  Per-object (model, texture)
    VkDescriptorSetLayout object;
};

void create_frame_descriptor_set_layout(
    VkDevice device,
    VkDescriptorSetLayout& layout
);

void create_object_descriptor_set_layout(
    VkDevice device,
    VkDescriptorSetLayout& layout
);

void create_descriptor_set_layouts(
    VkDevice device,
    DescriptorSetLayouts& layouts
);
}
