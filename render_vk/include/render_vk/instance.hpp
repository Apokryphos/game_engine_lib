#pragma once

#include "render_vk/vulkan.hpp"
#include <vector>

namespace render_vk
{
bool create_instance(
    VkInstance& instance,
    const std::vector<const char*>& validation_layers,
    VkDebugUtilsMessengerEXT& debug_messenger
);
}
