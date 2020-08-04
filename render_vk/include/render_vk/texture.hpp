#pragma once

#include "render_vk/vulkan.hpp"
#include <string>

namespace render
{
struct TextureLoadArgs;
}

namespace render_vk
{
class VulkanQueue;

typedef uint32_t TextureId;

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
    const render::TextureLoadArgs& args,
    Texture& texture
);

void destroy_texture(VkDevice device, const Texture& texture);
}
