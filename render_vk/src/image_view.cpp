#include "render_vk/vulkan.hpp"
#include <stdexcept>

namespace render_vk
{
//  ----------------------------------------------------------------------------
VkImageView create_image_view(
    VkDevice device,
    VkImage image,
    VkFormat format,
    uint32_t mip_levels,
    VkImageAspectFlags aspect_flags
) {
    VkImageViewCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    create_info.image = image;

    create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    create_info.format = format;

    create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

    create_info.subresourceRange.aspectMask = aspect_flags;
    create_info.subresourceRange.baseMipLevel = 0;
    create_info.subresourceRange.levelCount = mip_levels;
    create_info.subresourceRange.baseArrayLayer = 0;
    create_info.subresourceRange.layerCount = 1;

    VkImageView image_view;
    if (vkCreateImageView(
        device,
        &create_info,
        nullptr,
        &image_view
    ) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create image views.");
    }

    return image_view;
}
}
