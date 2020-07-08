#pragma once

#include "render_vk/uniform.hpp"
#include "render_vk/vulkan.hpp"
#include "render_vk/vulkan_swapchain.hpp"
#include <glm/mat4x4.hpp>
#include <cstdint>
#include <vector>

namespace render_vk
{
struct DrawModelCommand
{
    uint32_t index_count;
    glm::mat4x4 model;
    glm::mat4x4 view;
    glm::mat4x4 proj;
    VkBuffer vertex_buffer;
    VkBuffer index_buffer;
};

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

bool create_command_buffers(
    VkDevice device,
    VkCommandPool command_pool,
    const VulkanSwapchain& swapchain,
    std::vector<VkCommandBuffer>& command_buffers
);

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
);
}
