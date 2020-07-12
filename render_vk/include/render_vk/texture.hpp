#pragma once

#include "render_vk/vulkan.hpp"
#include <string>

namespace render_vk
{
class Texture
{
public:
    // uint32_t width;
    // uint32_t height;
    // uint32_t mip_levels;

    VkImage image;
    VkDeviceMemory image_memory;
    VkImageLayout layout;
    VkImageView view;
    VkSampler sampler;
};

void create_texture(
    VkPhysicalDevice physical_device,
    VkDevice device,
    VkQueue graphics_queue,
    VkCommandPool command_pool,
    const std::string& filename,
    Texture& texture
);

void create_image(
    VkPhysicalDevice physical_device,
    VkDevice device,
    uint32_t width,
    uint32_t height,
    VkFormat format,
    VkImageTiling tiling,
    VkImageUsageFlags usage,
    VkMemoryPropertyFlags properties,
    VkImage& image,
    VkDeviceMemory& image_memory
);

void create_texture_image(
    VkPhysicalDevice physical_device,
    VkDevice device,
    VkQueue transfer_queue,
    VkCommandPool command_pool,
    const std::string& filename,
    VkImage& texture_image,
    VkDeviceMemory& texture_image_memory
);

void create_texture_image_view(
    VkDevice device,
    VkImage& texture_image,
    VkImageView& texture_image_view
);

void create_texture_sampler(
    VkDevice device,
    VkSampler& texture_sampler
);

void destroy_texture(VkDevice device, const Texture& texture);

void transition_image_layout(
    VkDevice device,
    VkQueue transfer_queue,
    VkCommandPool command_pool,
    VkImage image,
    VkFormat format,
    VkImageLayout old_layout,
    VkImageLayout new_layout
);
}
