#pragma once

#include "render_vk/vulkan.hpp"
#include "render_vk/vulkan_queue.hpp"
#include <string>

namespace render_vk
{
static const uint32_t MAX_TEXTURES = 4096;

class Texture
{
public:
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
    // VkQueue graphics_queue,
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

void create_texture_image(
    VkPhysicalDevice physical_device,
    VkDevice device,
    VkQueue transfer_queue,
    VkCommandPool command_pool,
    const std::string& filename,
    VkImage& texture_image,
    uint32_t& mip_levels,
    VkDeviceMemory& texture_image_memory
);

void create_texture_image_view(
    VkDevice device,
    uint32_t mip_levels,
    VkImage& texture_image,
    VkImageView& texture_image_view
);

void create_texture_sampler(
    VkDevice device,
    uint32_t mipmap_levels,
    VkSampler& texture_sampler
);

void destroy_texture(VkDevice device, const Texture& texture);

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
