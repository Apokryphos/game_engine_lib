#pragma once

#include "render_vk/vulkan.hpp"
#include <glm/mat4x4.hpp>
#include <vector>

namespace render_vk
{
struct VulkanSwapchain;

// struct UniformBufferObject
// {
//     glm::mat4 model;
//     glm::mat4 view;
//     glm::mat4 proj;
// };

void create_uniform_buffers(
    VkPhysicalDevice physical_device,
    VkDevice device,
    const VulkanSwapchain& swapchain,
    std::vector<VkBuffer>& uniform_buffers,
    std::vector<VkDeviceMemory>& uniform_buffers_memory
);

void create_descriptor_set_layout(
    VkDevice device,
    VkDescriptorSetLayout& layout
);
}
