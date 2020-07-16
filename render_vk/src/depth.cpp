#include "render_vk/image_view.hpp"
#include "render_vk/texture.hpp"
#include "render_vk/vulkan.hpp"
#include "render_vk/vulkan_queue.hpp"
#include "render_vk/vulkan_swapchain.hpp"
#include <stdexcept>
#include <vector>

namespace render_vk
{
//  ----------------------------------------------------------------------------
bool has_stencil_component(VkFormat format) {
    return (
        format == VK_FORMAT_D32_SFLOAT_S8_UINT ||
        format == VK_FORMAT_D24_UNORM_S8_UINT
    );
}

//  ----------------------------------------------------------------------------
VkFormat find_supported_format(
    VkPhysicalDevice physical_device,
    const std::vector<VkFormat>& canidates,
    VkImageTiling tiling,
    VkFormatFeatureFlags features
) {
    for (VkFormat format : canidates) {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(physical_device, format, &props);

        if (tiling == VK_IMAGE_TILING_LINEAR &&
            (props.linearTilingFeatures & features) == features
        ) {
            return format;
        } else if (
            tiling == VK_IMAGE_TILING_OPTIMAL &&
            (props.optimalTilingFeatures & features) == features
        ) {
            return format;
        }
    }

    throw std::runtime_error("Failed to find supported format.");
}

//  ----------------------------------------------------------------------------
VkFormat find_depth_format(VkPhysicalDevice physical_device) {
    return find_supported_format(
        physical_device,
        {
            VK_FORMAT_D32_SFLOAT,
            VK_FORMAT_D32_SFLOAT_S8_UINT,
            VK_FORMAT_D24_UNORM_S8_UINT
        },
        VK_IMAGE_TILING_OPTIMAL,
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
    );
}

//  ----------------------------------------------------------------------------
void create_depth_resources(
    VkPhysicalDevice physical_device,
    VkDevice device,
    VulkanQueue& transfer_queue,
    VkCommandPool command_pool,
    const VulkanSwapchain& swapchain,
    VkImage& depth_image,
    VkImageView& depth_image_view,
    VkDeviceMemory& depth_image_memory
) {
    VkFormat depth_format = find_depth_format(physical_device);

    create_image(
        physical_device,
        device,
        swapchain.extent.width,
        swapchain.extent.height,
        1,
        depth_format,
        VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        depth_image,
        depth_image_memory
    );

    depth_image_view = create_image_view(
        device,
        depth_image,
        depth_format,
        1,
        VK_IMAGE_ASPECT_DEPTH_BIT
    );

    transition_image_layout(
        device,
        transfer_queue,
        command_pool,
        depth_image,
        depth_format,
        1,
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
    );
}
}
