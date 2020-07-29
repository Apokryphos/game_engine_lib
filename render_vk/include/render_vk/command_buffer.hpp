#pragma once

#include "render_vk/vulkan.hpp"
#include "render_vk/vulkan_swapchain.hpp"
#include <glm/mat4x4.hpp>
#include <cstdint>
#include <vector>

namespace render_vk
{
VkCommandBuffer begin_single_time_commands(
    VkDevice device,
    VkCommandPool command_pool
);

void end_single_time_commands(
    VkDevice device,
    VkCommandPool command_pool,
    VkCommandBuffer command_buffer,
    VkQueue transfer_queue
);

void create_command_buffers(
    VkDevice device,
    VkCommandPool command_pool,
    const uint32_t& swapchain_image_count,
    std::vector<VkCommandBuffer>& command_buffers
);

void create_primary_command_buffer(
    VkDevice device,
    VkCommandPool command_pool,
    VkCommandBuffer& command_buffer,
    const char* debug_name
);

void create_secondary_command_buffer(
    const VkDevice device,
    const VkCommandPool command_pool,
    VkCommandBuffer& command_buffer,
    const char* debug_name
);

void create_secondary_command_buffers(
    VkDevice device,
    VkCommandPool command_pool,
    const uint32_t& swapchain_image_count,
    std::vector<VkCommandBuffer>& command_buffers
);

void record_primary_command_buffer(
    VkRenderPass render_pass,
    VkPipelineLayout pipeline_layout,
    VkExtent2D extent,
    VkFramebuffer framebuffer,
    const std::vector<VkCommandBuffer>& secondary_command_buffers,
    VkCommandBuffer& command_buffer
);
}
