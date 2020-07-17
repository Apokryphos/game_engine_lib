#include "render_vk/texture.hpp"
#include "render_vk/vulkan.hpp"
#include <vector>

namespace render_vk
{
void create_descriptor_sets(
    VkDevice device,
    const uint32_t swapchain_image_count,
    VkDescriptorSetLayout descriptor_set_layout,
    VkDescriptorPool descriptor_pool,
    std::vector<VkDescriptorSet>& descriptor_sets
);

void update_descriptor_sets(
    VkDevice device,
    const uint32_t swapchain_image_count,
    std::vector<Texture>& textures,
    VkBuffer frame_uniform_buffer,
    VkBuffer object_uniform_buffer,
    size_t frame_ubo_size,
    std::vector<VkDescriptorSet>& descriptor_sets
);
}
