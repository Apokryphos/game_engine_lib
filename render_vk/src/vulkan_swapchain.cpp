#include "common/log.hpp"
#include "render_vk/image_view.hpp"
#include "render_vk/queue_family.hpp"
#include "render_vk/vulkan_swapchain.hpp"

using namespace common;

namespace render_vk
{
//  ----------------------------------------------------------------------------
static VkSurfaceFormatKHR choose_swap_surface_format(
    const std::vector<VkSurfaceFormatKHR>& available_formats
) {
    for (const auto& format : available_formats) {
        if (format.format == VK_FORMAT_B8G8R8A8_SRGB &&
            format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR
        ) {
            return format;
        }
    }

    return available_formats.at(0);
}

//  ----------------------------------------------------------------------------
static VkPresentModeKHR choose_swap_present_mode(
    const std::vector<VkPresentModeKHR>& available_present_modes
) {
    //  Check for preferred present mode
    for (const auto& present_mode : available_present_modes) {
        if (present_mode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return present_mode;
        }
    }

    //  TODO: VK_PRESENT_MODE_MAILBOX_KHR is preferred but seems to cause
    //  display to black out on development system (Fedora 32 / AMD RX 5600).
    return VK_PRESENT_MODE_FIFO_KHR;
}

//  ----------------------------------------------------------------------------
static VkExtent2D choose_swap_extent(
    const VkSurfaceCapabilitiesKHR& capabilities,
    const uint32_t width,
    const uint32_t height
) {
    if (capabilities.currentExtent.width != UINT32_MAX) {
        return capabilities.currentExtent;
    } else {
        VkExtent2D actual_extent = { width, height };

        actual_extent.width = std::clamp(
                capabilities.minImageExtent.width,
                capabilities.maxImageExtent.width,
                actual_extent.width
        );

        actual_extent.height = std::max(
            capabilities.minImageExtent.height,
            std::min(capabilities.maxImageExtent.height, actual_extent.height)
        );

        return actual_extent;
    }
}

//  ----------------------------------------------------------------------------
VulkanSwapchainSupport query_swapchain_support(
    VkPhysicalDevice device,
    VkSurfaceKHR surface
) {
    VulkanSwapchainSupport support;

    //  Basic surface capabilities
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &support.capabilities);

    //  Supported surface formats
    uint32_t format_count;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &format_count, nullptr);

    if (format_count != 0) {
        support.formats.resize(format_count);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &format_count, support.formats.data());
    }

    //  Supported presentation modes
    uint32_t present_mode_count;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &present_mode_count, nullptr);

    if (present_mode_count != 0) {
        support.present_modes.resize(present_mode_count);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &present_mode_count, support.present_modes.data());
    }

    return support;
}

//  ----------------------------------------------------------------------------
static bool create_image_views(VkDevice device, VulkanSwapchain& swapchain) {
    swapchain.image_views.resize(swapchain.images.size());
    for (size_t n = 0; n < swapchain.images.size(); ++n) {
        swapchain.image_views[n] = create_image_view(
            device,
            swapchain.images[n],
            swapchain.format,
            VK_IMAGE_ASPECT_COLOR_BIT
        );
    }

    return true;
}

//  ----------------------------------------------------------------------------
bool create_swapchain(
    VkDevice device,
    VkPhysicalDevice physical_device,
    VkSurfaceKHR surface,
    const int width,
    const int height,
    VulkanSwapchain& swapchain
) {
    VulkanSwapchainSupport support = query_swapchain_support(physical_device, surface);

    VkSurfaceFormatKHR surface_format = choose_swap_surface_format(support.formats);
    VkPresentModeKHR present_mode = choose_swap_present_mode(support.present_modes);
    VkExtent2D extent = choose_swap_extent(support.capabilities, width, height);

    //  Request one more image than minimum
    uint32_t image_count = support.capabilities.minImageCount + 1;

    if (support.capabilities.maxImageCount > 0 &&
        image_count > support.capabilities.maxImageCount
    ) {
        image_count = support.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    create_info.surface = surface;

    create_info.minImageCount = image_count;
    create_info.imageFormat = surface_format.format;
    create_info.imageColorSpace = surface_format.colorSpace;
    create_info.imageExtent = extent;
    create_info.imageArrayLayers = 1;
    create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    QueueFamilyIndices indices = find_queue_families(physical_device, surface);
    uint32_t queueFamilyIndices[] = {
        indices.graphics_family.value(),
        indices.present_family.value()
    };

    if (indices.graphics_family != indices.present_family) {
        create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        create_info.queueFamilyIndexCount = 2;
        create_info.pQueueFamilyIndices = queueFamilyIndices;
    } else {
        create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        create_info.queueFamilyIndexCount = 0; // Optional
        create_info.pQueueFamilyIndices = nullptr; // Optional
    }

    create_info.preTransform = support.capabilities.currentTransform;
    create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    create_info.presentMode = present_mode;
    create_info.clipped = VK_TRUE;
    create_info.oldSwapchain = VK_NULL_HANDLE;

    if (vkCreateSwapchainKHR(device, &create_info, nullptr, &swapchain.swapchain) != VK_SUCCESS) {
        log_error("Failed to create swap chain.");
        return false;
    }

    vkGetSwapchainImagesKHR(device, swapchain.swapchain, &image_count, nullptr);
    swapchain.images.resize(image_count);

    vkGetSwapchainImagesKHR(device, swapchain.swapchain, &image_count, swapchain.images.data());
    swapchain.format = surface_format.format;
    swapchain.extent = extent;

    if (!create_image_views(device, swapchain)) {
        return false;
    }

    return true;
}
}
