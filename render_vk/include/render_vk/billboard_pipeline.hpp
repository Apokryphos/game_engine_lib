#pragma once

#include "render_vk/descriptor_set_layout.hpp"
#include "render_vk/vulkan.hpp"
#include "render_vk/vulkan_swapchain.hpp"
#include <glm/vec2.hpp>
#include <vector>

namespace render_vk
{
struct BillboardPipeline
{
    VkPipelineLayout layout;
    VkPipeline pipeline;
};

void create_billboard_pipeline(
    VkDevice device,
    const VulkanSwapchain& swapchain,
    VkRenderPass render_pass,
    const DescriptorSetLayouts& descriptor_set_layouts,
    BillboardPipeline& billboard_pipeline
);
}
