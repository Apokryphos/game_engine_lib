#pragma once

#include "render_vk/vulkan.hpp"
#include <vector>

namespace render_vk
{
bool init_device(
    VkInstance instance,
    VkPhysicalDevice& physical_device,
    VkSurfaceKHR surface,
    const std::vector<const char*>& device_extensions
);

bool create_logical_device(
    VkPhysicalDevice physical_device,
    const VkSurfaceKHR& surface,
    const std::vector<const char*>& validation_layers,
    const std::vector<const char*>& device_extensions,
    VkDevice& device,
    VkQueue& graphics_queue,
    VkQueue& present_queue
);
}
