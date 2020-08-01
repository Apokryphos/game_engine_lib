#include "render_vk/command_buffer.hpp"
#include "render_vk/debug_utils.hpp"
#include "render_vk/imgui/imgui_vk.hpp"
#include "render_vk/vulkan.hpp"
#include "render_vk/vulkan_swapchain.hpp"
#include <stdexcept>

namespace render_vk
{
//  ----------------------------------------------------------------------------
VkCommandBuffer begin_single_time_commands(
    VkDevice device,
    VkCommandPool command_pool
) {
    VkCommandBufferAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandPool = command_pool;
    alloc_info.commandBufferCount = 1;

    //  Create temporary command buffer
    VkCommandBuffer command_buffer;
    VK_CHECK_RESULT(vkAllocateCommandBuffers(device, &alloc_info, &command_buffer));

    VkCommandBufferBeginInfo begin_info{};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    VK_CHECK_RESULT(vkBeginCommandBuffer(command_buffer, &begin_info));

    return command_buffer;
}

//  ----------------------------------------------------------------------------
void create_command_buffers(
    VkDevice device,
    VkCommandPool command_pool,
    const uint32_t& swapchain_image_count,
    std::vector<VkCommandBuffer>& command_buffers
) {
    command_buffers.resize(swapchain_image_count);

    VkCommandBufferAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.commandPool = command_pool;
    alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandBufferCount = (uint32_t)command_buffers.size();

    if (vkAllocateCommandBuffers(
        device,
        &alloc_info,
        command_buffers.data()
    ) != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate command buffers.");
    }
}

//  ----------------------------------------------------------------------------
void create_primary_command_buffer(
    VkDevice device,
    VkCommandPool command_pool,
    VkCommandBuffer& command_buffer,
    const char* debug_name
) {
    VkCommandBufferAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.commandPool = command_pool;
    alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandBufferCount = 1;

    if (vkAllocateCommandBuffers(
        device,
        &alloc_info,
        &command_buffer
    ) != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate command buffers.");
    }

    set_debug_name(
        device,
        VK_OBJECT_TYPE_COMMAND_BUFFER,
        command_buffer,
        debug_name
    );
}

//  ----------------------------------------------------------------------------
void create_secondary_command_buffers(
    VkDevice device,
    VkCommandPool command_pool,
    const uint32_t& swapchain_image_count,
    std::vector<VkCommandBuffer>& command_buffers
) {
    command_buffers.resize(swapchain_image_count);

    VkCommandBufferAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.commandPool = command_pool;
    alloc_info.level = VK_COMMAND_BUFFER_LEVEL_SECONDARY;
    alloc_info.commandBufferCount = (uint32_t)command_buffers.size();

    if (vkAllocateCommandBuffers(
        device,
        &alloc_info,
        command_buffers.data()
    ) != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate secondary command buffers.");
    }
}

//  ----------------------------------------------------------------------------
void create_secondary_command_buffer(
    const VkDevice device,
    const VkCommandPool command_pool,
    VkCommandBuffer& command_buffer,
    const char* debug_name
) {
    VkCommandBufferAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.commandPool = command_pool;
    alloc_info.level = VK_COMMAND_BUFFER_LEVEL_SECONDARY;
    alloc_info.commandBufferCount = 1;

    if (vkAllocateCommandBuffers(device, &alloc_info, &command_buffer) != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate secondary command buffers.");
    }

    set_debug_name(
        device,
        VK_OBJECT_TYPE_COMMAND_BUFFER,
        command_buffer,
        debug_name
    );
}

//  ----------------------------------------------------------------------------
void record_primary_command_buffer(
    VkRenderPass render_pass,
    VkExtent2D extent,
    VkFramebuffer framebuffer,
    const std::vector<VkCommandBuffer>& secondary_command_buffers,
    VkCommandBuffer& command_buffer
) {
    //  Record command buffer
    VkCommandBufferBeginInfo begin_info{};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
    begin_info.pInheritanceInfo = nullptr; // Optional

    if (vkBeginCommandBuffer(command_buffer, &begin_info) != VK_SUCCESS) {
        throw std::runtime_error("Failed to begin recording command buffer.");
    }

    //  Start render pass
    VkRenderPassBeginInfo render_pass_info{};
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    render_pass_info.renderPass = render_pass;
    render_pass_info.framebuffer = framebuffer;
    render_pass_info.renderArea.offset = {0, 0};
    render_pass_info.renderArea.extent = extent;

    //  Order of clear values should match order of attachments
    std::array<VkClearValue, 2> clear_values{};
    clear_values[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
    clear_values[1].depthStencil = { 1.0f, 0 };

    render_pass_info.clearValueCount = static_cast<uint32_t>(clear_values.size());
    render_pass_info.pClearValues = clear_values.data();

    //  Begin render pass
    vkCmdBeginRenderPass(command_buffer, &render_pass_info, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);

    //  Execute secondary command buffers
    if (!secondary_command_buffers.empty()) {
        vkCmdExecuteCommands(
            command_buffer,
            static_cast<uint32_t>(secondary_command_buffers.size()),
            secondary_command_buffers.data()
        );
    }

    //  Start next subpass for ImGui
    vkCmdNextSubpass(command_buffer, VK_SUBPASS_CONTENTS_INLINE);

    //  ImGui
    begin_debug_marker(command_buffer, "ImGui", DEBUG_MARKER_COLOR_ORANGE);
    imgui_vulkan_render_frame(command_buffer);
    end_debug_marker(command_buffer);

    //  End render pass
    vkCmdEndRenderPass(command_buffer);

    if (vkEndCommandBuffer(command_buffer) != VK_SUCCESS) {
        throw std::runtime_error("Failed to record command buffer.");
    }
}
}
