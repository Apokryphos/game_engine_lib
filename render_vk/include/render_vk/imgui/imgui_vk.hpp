#pragma once

struct GLFWwindow;

#include "render_vk/vulkan.hpp"

namespace render_vk
{
//  Initializes ImGui.
//  Currently, ImGui will use the same render pass and command pool as the
//  engine.
void imgui_vulkan_init(
    VkInstance instance,
    VkPhysicalDevice physical_device,
    VkDevice device,
    VkQueue queue,
    VulkanSwapchain& swapchain,
    VkRenderPass render_pass,
    VkCommandPool command_pool
);

//  Call when resources should be cleaned-up after swapchain is recreated.
void imgui_vulkan_cleanup_swapchain(VkDevice device);

//  Call when resources should be recreated after swapchain is recreated.
void imgui_vulkan_recreate_swapchain(
    VkInstance instance,
    VkPhysicalDevice physical_device,
    VkDevice device,
    VkQueue queue,
    VulkanSwapchain& swapchain,
    VkRenderPass render_pass,
    VkCommandPool command_pool
);

void imgui_vulkan_render_frame(VkCommandBuffer command_buffer);
void imgui_vulkan_shutdown(VkDevice device);
}
