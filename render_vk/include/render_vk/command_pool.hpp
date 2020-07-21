#pragma once

#include "render_vk/vulkan.hpp"

namespace render_vk
{
void create_command_pool(
    VkDevice device,
    VkPhysicalDevice physical_device,
    VkCommandPool& command_pool,
    const char* debug_name
);
}
