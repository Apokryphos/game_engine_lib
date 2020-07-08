#pragma once

#include "render_vk/vulkan.hpp"
#include <cstdint>
#include <optional>

namespace render_vk
{
struct QueueFamilyIndices
{
    std::optional<uint32_t> graphics_family;
    std::optional<uint32_t> present_family;

    bool is_complete() const {
        return graphics_family.has_value() && present_family.has_value();
    }
};

QueueFamilyIndices find_queue_families(
    VkPhysicalDevice device,
    VkSurfaceKHR surface
);
}
