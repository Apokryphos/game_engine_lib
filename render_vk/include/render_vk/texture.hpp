#pragma once

#include "render_vk/vulkan.hpp"
#include "render_vk/vulkan_queue.hpp"
#include <string>

namespace render_vk
{
typedef uint32_t TextureId;

static const uint32_t MAX_TEXTURES = 4096;

class Texture
{
public:
    TextureId id;
    // uint32_t width;
    // uint32_t height;
    uint32_t mip_levels;
    VkImage image;
    VkDeviceMemory image_memory;
    VkImageLayout layout;
    VkImageView view;
    VkSampler sampler;
};

void create_texture(
    VkPhysicalDevice physical_device,
    VkDevice device,
    VulkanQueue& transfer_queue,
    VkCommandPool command_pool,
    const std::string& filename,
    Texture& texture
);

void create_image(
    VkPhysicalDevice physical_device,
    VkDevice device,
    uint32_t width,
    uint32_t height,
    uint32_t mip_levels,
    VkFormat format,
    VkImageTiling tiling,
    VkImageUsageFlags usage,
    VkMemoryPropertyFlags properties,
    VkImage& image,
    VkDeviceMemory& image_memory
);

void destroy_texture(VkDevice device, const Texture& texture);

//  Submits transition image layout commands to a queue using single-time
//  commands.
void transition_image_layout(
    VkDevice device,
    VulkanQueue& transfer_queue,
    VkCommandPool command_pool,
    VkImage image,
    VkFormat format,
    uint32_t mip_levels,
    VkImageLayout old_layout,
    VkImageLayout new_layout
);
}
