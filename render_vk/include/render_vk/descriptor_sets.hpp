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
    VkImageView texture_image_view,
    VkSampler texture_sampler,
    VkBuffer uniform_buffer,
    // const std::vector<VkBuffer>& uniform_buffers,
    std::vector<VkDescriptorSet>& descriptor_sets
);
}
