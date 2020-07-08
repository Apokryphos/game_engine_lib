#pragma once

#include "render_vk/vulkan.hpp"

namespace render_vk
{
struct VulkanSwapchain;

//  ----------------------------------------------------------------------------
void create_descriptor_pool(
    VkDevice device,
    const VulkanSwapchain& swapchain,
    VkDescriptorPool& descriptor_pool
);
}
