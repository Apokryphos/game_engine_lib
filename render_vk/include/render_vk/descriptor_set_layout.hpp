#pragma once

#include "render_vk/vulkan.hpp"
#include <glm/mat4x4.hpp>
#include <vector>

namespace render_vk
{
struct VulkanSwapchain;

void create_descriptor_set_layout(
    VkDevice device,
    VkDescriptorSetLayout& layout
);
}
