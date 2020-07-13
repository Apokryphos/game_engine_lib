#pragma once

#include "render_vk/vulkan.hpp"
#include <glm/mat4x4.hpp>

namespace render_vk
{
void get_dynamic_buffer_align_and_size(
    VkPhysicalDevice physical_device,
    const size_t object_count,
    const size_t uniform_buffer_size,
    size_t& dynamic_align,
    size_t& buffer_size
);
}
