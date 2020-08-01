#include "render_vk/buffer.hpp"
#include "render_vk/command_buffer.hpp"
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
    VkCommandBuffer command_buffer = begin_single_time_commands(m_device, command_pool);

    VkBufferCopy copy_region{};
    copy_region.srcOffset = 0; // Optional
    copy_region.dstOffset = 0; // Optional
    copy_region.size = size;
    vkCmdCopyBuffer(command_buffer, src, dst, 1, &copy_region);

    end_single_time_commands(command_pool, command_buffer);
}

//  ----------------------------------------------------------------------------
void VulkanQueue::end_single_time_commands(
    VkCommandPool command_pool,
    VkCommandBuffer command_buffer
) {
    VK_CHECK_RESULT(vkEndCommandBuffer(command_buffer));

    VkSubmitInfo submit_info{};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &command_buffer;

     //  Create fence to signal when complete
    VkFenceCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    VkFence fence;
    VK_CHECK_RESULT(vkCreateFence(m_device, &create_info, nullptr, &fence));

    std::lock_guard<std::mutex> lock(m_queue_mutex);
    {
        VK_CHECK_RESULT(vkQueueSubmit(m_queue, 1, &submit_info, fence));
        VK_CHECK_RESULT(vkWaitForFences(m_device, 1, &fence, VK_TRUE, UINT64_MAX));
    }

    vkDestroyFence(m_device, fence, nullptr);
    vkFreeCommandBuffers(m_device, command_pool, 1, &command_buffer);
}

//  ----------------------------------------------------------------------------
VkResult VulkanQueue::present(const VkPresentInfoKHR& present_info) {
    std::lock_guard<std::mutex> lock(m_queue_mutex);
    VkResult result = vkQueuePresentKHR(m_queue, &present_info);
    return result;
}

//  ----------------------------------------------------------------------------
VkResult VulkanQueue::submit(
    uint32_t submit_info_count,
    const VkSubmitInfo& submit_info,
    VkFence fence
) {
    std::lock_guard<std::mutex> lock(m_queue_mutex);
    VkResult result = vkQueueSubmit(
        m_queue,
        1,
        &submit_info,
        fence
    );
    return result;
}
}
