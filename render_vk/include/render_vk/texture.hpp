#pragma once

#include "render_vk/texture_id.hpp"
#include "render_vk/vulkan.hpp"
#include <string>

namespace assets
{
struct TextureCreateArgs;
}

namespace render_vk
{
class VulkanQueue;

static const uint32_t MAX_TEXTURES = 4096;

class Texture
{
public:
    TextureId id;
    uint32_t width;
    uint32_t height;
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
    const assets::TextureCreateArgs& args,
    Texture& texture
);

void destroy_texture(VkDevice device, const Texture& texture);
}
