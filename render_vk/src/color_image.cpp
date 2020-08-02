#include "render_vk/color_image.hpp"
#include "render_vk/image.hpp"
#include "render_vk/image_view.hpp"
#include "render_vk/vulkan_swapchain.hpp"

namespace render_vk
{
//  ----------------------------------------------------------------------------
void create_color_resources(
    VkPhysicalDevice physical_device,
    VkDevice device,
    const VulkanSwapchain& swapchain,
    VkSampleCountFlagBits msaa_sample_count,
    ColorImage& color_image
) {
    VkFormat color_format = swapchain.format;

    create_image(
        physical_device,
        device,
        swapchain.extent.width,
        swapchain.extent.height,
        1,
        msaa_sample_count,
        color_format,
        VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        color_image.image,
        color_image.memory
    );

    color_image.view = create_image_view(
        device,
        color_image.image,
        color_format,
        1,
        VK_IMAGE_ASPECT_COLOR_BIT
    );
}
}
