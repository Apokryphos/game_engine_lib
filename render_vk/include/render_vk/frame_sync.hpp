#pragma once

#include "render_vk/vulkan.hpp"
#include <vector>

namespace render_vk
{
struct VulkanSwapchain;

//  Contains frame synchronization objects
struct FrameSync
{
    std::vector<VkSemaphore> image_ready;
    std::vector<VkSemaphore> render_done;
    std::vector<VkFence> in_flight_fences;
    std::vector<VkFence> images_in_flight;
};

FrameSync create_frame_sync(
    VkDevice device,
    const VulkanSwapchain& swapchain,
    uint32_t frame_count
);

void create_sync_objects(
    VkDevice device,
    const VulkanSwapchain& swapchain,
    uint32_t frame_count,
    std::vector<VkSemaphore>& image_available_semaphores,
    std::vector<VkSemaphore>& render_finished_semaphores,
    std::vector<VkFence>& in_flight_fences,
    std::vector<VkFence>& images_in_flight
);
}
