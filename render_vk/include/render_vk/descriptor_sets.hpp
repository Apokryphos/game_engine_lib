#pragma once

#include "render_vk/descriptor_set_layout.hpp"
#include "render_vk/texture.hpp"
#include "render_vk/vulkan.hpp"
#include <vector>

namespace render_vk
{
// struct DescriptorSets
// {
//     std::vector<VkDescriptorSet> frame_sets;
//     // std::vector<VkDescriptorSet> object_sets;
//     std::vector<VkDescriptorSet> texture_sets;
//     const static uint32_t SET_COUNT = 2;
// };

// void create_descriptor_sets(
//     VkDevice device,
//     const uint32_t swapchain_image_count,
//     const DescriptorSetLayouts& descriptor_set_layouts,
//     VkDescriptorPool descriptor_pool,
//     DescriptorSets& descriptor_sets
// );

// void create_descriptor_sets(
//     VkDevice device,
//     const uint32_t swapchain_image_count,
//     VkDescriptorSetLayout descriptor_set_layout,
//     VkDescriptorPool descriptor_pool,
//     std::vector<VkDescriptorSet>& descriptor_sets
// );

// void update_descriptor_sets(
//     VkDevice device,
//     const uint32_t swapchain_image_count,
//     std::vector<Texture>& textures,
//     VkBuffer frame_uniform_buffer,
//     VkBuffer object_uniform_buffer,
//     size_t frame_ubo_size,
//     DescriptorSets& descriptor_sets
// );

// void update_descriptor_sets(
//     VkDevice device,
//     const uint32_t swapchain_image_count,
//     std::vector<Texture>& textures,
//     VkBuffer frame_uniform_buffer,
//     VkBuffer object_uniform_buffer,
//     size_t frame_ubo_size,
//     std::vector<VkDescriptorSet>& descriptor_sets
// );
}
