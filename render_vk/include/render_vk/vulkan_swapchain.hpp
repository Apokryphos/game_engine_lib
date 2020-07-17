#pragma once

#include "render_vk/vulkan.hpp"
#include <vector>

namespace render_vk
{
//  Contains a swapchain (and its images, image views, framebuffers, etc.)
struct VulkanSwapchain
{
    //  Image format
    VkFormat format = VK_FORMAT_UNDEFINED;
    //  Extent
    VkExtent2D extent = {0};
    //  Swapchain
    VkSwapchainKHR swapchain = VK_NULL_HANDLE;
    //  Swapchain images
    std::vector<VkImage> images;
    //  Swapchain image views
    std::vector<VkImageView> image_views;
    //  Framebuffers
    std::vector<VkFramebuffer> framebuffers;
};

struct VulkanSwapchainSupport {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> present_modes;
};

bool create_swapchain(
    VkDevice device,
    VkPhysicalDevice physical_device,
    VkSurfaceKHR surface,
    const int width,
    const int height,
    VulkanSwapchain& swapchain
);

VulkanSwapchainSupport query_swapchain_support(
    VkPhysicalDevice device,
    VkSurfaceKHR surface
);
}
