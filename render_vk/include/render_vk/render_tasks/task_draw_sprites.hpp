#pragma once

#include "render/sprite_batch.hpp"
#include "render_vk/vulkan_render_system.hpp"
#include "render_vk/vulkan.hpp"

namespace render_vk
{
struct SpritePipeline;

void task_draw_sprites(
    const VkRenderPass render_pass,
    const SpritePipeline& pipeline,
    ModelManager& model_mgr,
    const VulkanRenderSystem::FrameDescriptorObjects& descriptors,
    const std::vector<render::SpriteBatch>& batches,
    VkCommandBuffer command_buffer
);
}
