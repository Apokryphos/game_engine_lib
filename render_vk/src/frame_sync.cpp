#include "render_vk/frame_sync.hpp"
#include "render_vk/vulkan_swapchain.hpp"
#include <stdexcept>

namespace render_vk
{
//  ----------------------------------------------------------------------------
void create_sync_objects(
    VkDevice device,
    const VulkanSwapchain& swapchain,
    uint32_t frame_count,
    std::vector<VkSemaphore>& image_available_semaphores,
    std::vector<VkSemaphore>& render_finished_semaphores,
    std::vector<VkFence>& in_flight_fences,
    std::vector<VkFence>& images_in_flight
) {
    image_available_semaphores.resize(frame_count);
    render_finished_semaphores.resize(frame_count);
    in_flight_fences.resize(frame_count);
    images_in_flight.resize(swapchain.images.size(), VK_NULL_HANDLE);

    VkSemaphoreCreateInfo semaphore_info{};
    semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fence_info{};
    fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t n = 0; n < frame_count; ++n) {
        if (vkCreateSemaphore(device, &semaphore_info, nullptr, &image_available_semaphores[n]) != VK_SUCCESS ||
            vkCreateSemaphore(device, &semaphore_info, nullptr, &render_finished_semaphores[n]) != VK_SUCCESS ||
            vkCreateFence(device, &fence_info, nullptr, &in_flight_fences[n]) != VK_SUCCESS
        ) {
            throw std::runtime_error("Failed to create semaphores.");
        }
    }
}

//  ----------------------------------------------------------------------------
FrameSync create_frame_sync(
    VkDevice device,
    const VulkanSwapchain& swapchain,
    uint32_t frame_count
) {
    FrameSync frame_sync{};

    create_sync_objects(
        device,
        swapchain,
        frame_count,
        frame_sync.image_ready,
        frame_sync.render_done,
        frame_sync.in_flight_fences,
        frame_sync.images_in_flight
    );

    return frame_sync;
}
}
