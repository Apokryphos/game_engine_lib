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
    VkCommandBuffer& command_buffer
);

void create_secondary_command_buffer(
    VkDevice device,
    VkCommandPool command_pool,
    VkCommandBuffer& command_buffer
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
    VkPipeline graphics_pipeline,
    VkExtent2D extent,
    VkFramebuffer framebuffer,
    VkCommandBuffer& command_buffer,
    VkCommandBuffer secondary_command_buffer
);

// void record_secondary_command_buffer(
//     VkRenderPass render_pass,
//     VkPipelineLayout pipeline_layout,
//     VkPipeline graphics_pipeline,
//     const std::vector<DrawModelCommand>& draw_model_commands,
//     VkDescriptorSet descriptor_set,
//     VkExtent2D extent,
//     VkCommandBuffer& command_buffer,
//     size_t ubo_dynamic_align
// );
}
