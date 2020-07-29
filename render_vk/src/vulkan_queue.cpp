#include "render_vk/buffer.hpp"
#include "render_vk/vulkan_queue.hpp"

namespace render_vk
{
//  ----------------------------------------------------------------------------
void VulkanQueue::copy_buffer(
    VkCommandPool command_pool,
    VkBuffer src,
    VkBuffer dst,
    VkDeviceSize size
) {
    std::lock_guard<std::mutex> lock(m_queue_mutex);

    render_vk::copy_buffer(
        m_device,
        m_queue,
        command_pool,
        src,
        dst,
        size
    );
}

//  ----------------------------------------------------------------------------
void VulkanQueue::end_single_time_commands(
    VkCommandPool command_pool,
    VkCommandBuffer command_buffer,
    VkFence fence
) {
    vkEndCommandBuffer(command_buffer);

    VkSubmitInfo submit_info{};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &command_buffer;

    std::lock_guard<std::mutex> lock(m_queue_mutex);
    {
        vkQueueSubmit(m_queue, 1, &submit_info, fence);

        if (fence == VK_NULL_HANDLE) {
            vkQueueWaitIdle(m_queue);
        }
    }

    vkFreeCommandBuffers(m_device, command_pool, 1, &command_buffer);
}

//  ----------------------------------------------------------------------------
VkResult VulkanQueue::submit(
    uint32_t submit_info_count,
    const VkSubmitInfo& submit_info,
    VkFence fence
) {
    std::lock_guard<std::mutex> lock(m_queue_mutex);

    return vkQueueSubmit(
        m_queue,
        1,
        &submit_info,
        fence
    );
}
}
