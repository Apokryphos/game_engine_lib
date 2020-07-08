#pragma once

#include "render_vk/vulkan.hpp"
#include <vector>

namespace render_vk
{
//  Contains a swapchain (and its images, image views, framebuffers, etc.)
struct VulkanSwapchain
{
    //  Swapchain
    VkSwapchainKHR swapchain;
    //  Swapchain images
    std::vector<VkImage> images;
    //  Swapchain image views
    std::vector<VkImageView> image_views;
    //  Framebuffers
    std::vector<VkFramebuffer> framebuffers;
    //  Image format
    VkFormat format;
    //  Extent
    VkExtent2D extent;
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
