#pragma once

#include "render_vk/vulkan.hpp"

namespace render_vk
{
VkImageView create_image_view(
    VkDevice device,
    VkImage image,
    VkFormat format,
    VkImageAspectFlags aspect_flags
);
}
