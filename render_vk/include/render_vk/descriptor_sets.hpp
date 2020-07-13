#include "render_vk/vulkan.hpp"
#include <vector>

namespace render_vk
{
struct VulkanSwapchain;

void create_descriptor_sets(
    VkDevice device,
    const VulkanSwapchain swapchain,
    VkDescriptorSetLayout descriptor_set_layout,
    VkDescriptorPool descriptor_pool,
    std::vector<VkDescriptorSet>& descriptor_sets
);

void update_descriptor_sets(
    VkDevice device,
    const VulkanSwapchain swapchain,
    VkImageView texture_image_view,
    VkSampler texture_sampler,
    VkBuffer frame_uniform_buffer,
    VkBuffer object_uniform_buffer,
    size_t frame_ubo_size,
    std::vector<VkDescriptorSet>& descriptor_sets
);
}
