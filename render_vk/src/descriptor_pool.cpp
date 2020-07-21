#include "render_vk/vulkan.hpp"
#include <array>
#include <stdexcept>

namespace render_vk
{
//  ----------------------------------------------------------------------------
void create_descriptor_pool(
    VkDevice device,
    const uint32_t swapchain_image_count,
    const uint32_t sampler_count,
    VkDescriptorPool& descriptor_pool
) {
    const uint32_t max_sets = swapchain_image_count * 2;

    std::array<VkDescriptorPoolSize, 3> pool_sizes{};
    pool_sizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    pool_sizes[0].descriptorCount = swapchain_image_count;
    // pool_sizes[1].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    // pool_sizes[1].descriptorCount = swapchain_image_count;
    pool_sizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    pool_sizes[1].descriptorCount = sampler_count * swapchain_image_count;

    VkDescriptorPoolCreateInfo pool_info{};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.poolSizeCount = static_cast<uint32_t>(pool_sizes.size());
    pool_info.pPoolSizes = pool_sizes.data();
    pool_info.maxSets = max_sets;

    if (vkCreateDescriptorPool(device, &pool_info, nullptr, &descriptor_pool) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create descriptor pool.");
    }
}
}
