#include "common/log.hpp"
#include "render_vk/command_buffer.hpp"
#include "render_vk/debug_utils.hpp"
#include "render_vk/imgui/imgui_vk.hpp"
#include "render_vk/uniform.hpp"
#include "render_vk/vulkan.hpp"
#include "render_vk/vulkan_swapchain.hpp"

using namespace common;

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
    vkAllocateCommandBuffers(device, &alloc_info, &command_buffer);

    VkCommandBufferBeginInfo begin_info{};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(command_buffer, &begin_info);

    return command_buffer;
}

//  ----------------------------------------------------------------------------
void end_single_time_commands(
    VkDevice device,
    VkCommandPool command_pool,
    VkCommandBuffer command_buffer,
    VkQueue transfer_queue
) {
    vkEndCommandBuffer(command_buffer);

    VkSubmitInfo submit_info{};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &command_buffer;

    vkQueueSubmit(transfer_queue, 1, &submit_info, VK_NULL_HANDLE);

    //  TODO: Fences would permit multiple transfers
    vkQueueWaitIdle(transfer_queue);

    vkFreeCommandBuffers(device, command_pool, 1, &command_buffer);
}

//  ----------------------------------------------------------------------------
bool create_command_buffers(
    VkDevice device,
    VkCommandPool command_pool,
    const VulkanSwapchain& swapchain,
    std::vector<VkCommandBuffer>& command_buffers
) {
    command_buffers.resize(swapchain.framebuffers.size());

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
        log_debug("Failed to allocate command buffers.");
        return false;
    }

    return true;
}

//  ----------------------------------------------------------------------------
void record_command_buffer(
    VkRenderPass render_pass,
    VkPipelineLayout pipeline_layout,
    VkPipeline graphics_pipeline,
    std::vector<DrawModelCommand>& draw_model_commands,
    VkDescriptorSet descriptor_set,
    VkExtent2D extent,
    VkFramebuffer framebuffer,
    VkCommandBuffer& command_buffer,
    size_t ubo_dynamic_align
) {
    //  Record command buffer
    VkCommandBufferBeginInfo begin_info{};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags = 0; // Optional
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
    vkCmdBeginRenderPass(command_buffer, &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);

    //  Bind pipeline
    vkCmdBindPipeline(
        command_buffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        graphics_pipeline
    );

    //  Draw each model
    size_t model_index = 0;
    begin_debug_marker(command_buffer, "Draw Model", DEBUG_MARKER_COLOR_ORANGE);
    for (const DrawModelCommand cmd : draw_model_commands) {
        //  Bind vertex buffer
        vkCmdBindPipeline(
            command_buffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            graphics_pipeline
        );

        VkBuffer vertex_buffers[] = { cmd.vertex_buffer };
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(command_buffer, 0, 1, vertex_buffers, offsets);

        //  Bind index buffer
        vkCmdBindIndexBuffer(command_buffer, cmd.index_buffer, 0, VK_INDEX_TYPE_UINT32);

        //  Bind descriptor set
        // vkCmdBindDescriptorSets(
        //     command_buffer,
        //     VK_PIPELINE_BIND_POINT_GRAPHICS,
        //     pipeline_layout,
        //     0,
        //     1,
        //     &descriptor_set,
        //     0,
        //     nullptr
        // );

        // One dynamic offset per dynamic descriptor to offset into the ubo containing all model matrices
        const uint32_t dynamic_offset = model_index * static_cast<uint32_t>(ubo_dynamic_align);

        // Bind the descriptor set for rendering a mesh using the dynamic offset
        vkCmdBindDescriptorSets(
            command_buffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            pipeline_layout,
            0,
            1,
            &descriptor_set,
            1,
            &dynamic_offset
        );

        ++model_index;

        //  Draw
        vkCmdDrawIndexed(command_buffer, static_cast<uint32_t>(cmd.index_count), 1, 0, 0, 0);
    }
    end_debug_marker(command_buffer);

    //  ImGui
    imgui_vulkan_render_frame(command_buffer);

    //  End render pass
    vkCmdEndRenderPass(command_buffer);

    if (vkEndCommandBuffer(command_buffer) != VK_SUCCESS) {
        throw std::runtime_error("Failed to record command buffer.");
    }
}
}
