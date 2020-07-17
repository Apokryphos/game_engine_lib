#pragma once

#include "render_vk/vulkan.hpp"

namespace render_vk
{
//  ----------------------------------------------------------------------------
void create_descriptor_pool(
    VkDevice device,
    const uint32_t swapchain_image_count,
    VkDescriptorPool& descriptor_pool
);
}
