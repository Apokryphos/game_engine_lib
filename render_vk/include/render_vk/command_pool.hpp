#pragma once

#include "render_vk/vulkan.hpp"

namespace render_vk
{
bool create_command_pool(
    VkDevice device,
    VkPhysicalDevice physical_device,
    VkSurfaceKHR surface,
    VkCommandPool& command_pool
);
}
