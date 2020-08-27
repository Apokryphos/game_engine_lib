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
    TextureId id {0};
    uint32_t width {0};
    uint32_t height {0};
    uint32_t mip_levels {0};
    VkImage image {VK_NULL_HANDLE};
    VkDeviceMemory image_memory {VK_NULL_HANDLE};
    VkImageLayout layout {VK_IMAGE_LAYOUT_UNDEFINED};
    VkImageView view {VK_NULL_HANDLE};
    VkSampler sampler {VK_NULL_HANDLE};
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
