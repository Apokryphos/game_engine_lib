#include "common/log.hpp"
#include "render_vk/buffer.hpp"
#include "render_vk/command_buffer.hpp"
#include "render_vk/debug_utils.hpp"
#include "render_vk/image.hpp"
#include "render_vk/image_view.hpp"
#include "render_vk/texture.hpp"
#include "render_vk/vulkan_queue.hpp"
#include <lodepng.h>
#include <stdexcept>

using namespace common;

namespace render_vk
{
//  ----------------------------------------------------------------------------
static void generate_mipmaps(
    VkCommandBuffer command_buffer,
    VkImage image,
    int32_t texture_width,
    int32_t texture_height,
    uint32_t mipmap_levels
) {
    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.image = image;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    barrier.subresourceRange.levelCount = 1;

    int32_t mipmap_width = texture_width;
    int32_t mipmap_height = texture_height;

    for (uint32_t n = 1; n < mipmap_levels; ++n) {
        barrier.subresourceRange.baseMipLevel = n -1;
        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

        vkCmdPipelineBarrier(
            command_buffer,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            0,
            0,
            nullptr,
            0,
            nullptr,
            1,
            &barrier
        );

        VkImageBlit blit{};
        blit.srcOffsets[0] = { 0, 0, 0 };
        blit.srcOffsets[1] = { mipmap_width, mipmap_height, 1 };
        blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blit.srcSubresource.mipLevel = n - 1;
        blit.srcSubresource.baseArrayLayer = 0;
        blit.srcSubresource.layerCount = 1;
        blit.dstOffsets[0] = { 0, 0, 0 };
        blit.dstOffsets[1] = { mipmap_width > 1 ? mipmap_width / 2 : 1, mipmap_height > 1 ? mipmap_height / 2 : 1, 1 };
        blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blit.dstSubresource.mipLevel = n;
        blit.dstSubresource.baseArrayLayer = 0;
        blit.dstSubresource.layerCount = 1;

        vkCmdBlitImage(command_buffer,
            image,
            VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            image,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1,
            &blit,
            VK_FILTER_LINEAR
        );

        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        vkCmdPipelineBarrier(
            command_buffer,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
            0,
            0,
            nullptr,
            0,
            nullptr,
            1,
            &barrier
        );

        if (mipmap_width > 1) {
            mipmap_width /= 2;
        }
        if (mipmap_height > 1) {
            mipmap_height /= 2;
        }
    }

    barrier.subresourceRange.baseMipLevel = mipmap_levels - 1;
    barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    vkCmdPipelineBarrier(
        command_buffer,
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
        0,
        0,
        nullptr,
        0,
        nullptr,
        1,
        &barrier
    );
}

//  ----------------------------------------------------------------------------
static void copy_buffer_to_image(
    VkCommandBuffer command_buffer,
    VkBuffer buffer,
    VkImage image,
    uint32_t width,
    uint32_t height
) {
    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;

    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;

    region.imageOffset = {0, 0, 0};
    region.imageExtent = {
        width,
        height,
        1
    };

    vkCmdCopyBufferToImage(
        command_buffer,
        buffer,
        image,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        1,
        &region
    );
}

//  ----------------------------------------------------------------------------
static void create_texture_image(
    VkPhysicalDevice physical_device,
    VkDevice device,
    VulkanQueue& transfer_queue,
    VkCommandPool command_pool,
    VkSampleCountFlagBits msaa_sample_count,
    const std::string& filename,
    VkImage& texture_image,
    uint32_t& mip_levels,
    VkDeviceMemory& texture_image_memory
) {
    //  Decode PNG file
    unsigned width;
    unsigned height;
    std::vector<unsigned char> image;
    const auto error = lodepng::decode(image, width, height, filename);
    if (error != 0) {
        log_error(
            "Error decoding PNG '%s': %s",
            filename.c_str(),
            lodepng_error_text(error)
        );

        throw std::runtime_error("Failed to load texture image.");
    }

    mip_levels = static_cast<uint32_t>(
        std::floor(
            std::log2(std::max(width, height)))
        ) + 1;

    VkDeviceSize image_size = width * height * 4;

    VkBuffer staging_buffer;
    VkDeviceMemory staging_buffer_memory;

    create_buffer(
        physical_device,
        device,
        image_size,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        staging_buffer,
        staging_buffer_memory
    );

    void* data;
    vkMapMemory(device, staging_buffer_memory, 0, image_size, 0, &data);
    memcpy(data, image.data(), static_cast<size_t>(image_size));
    vkUnmapMemory(device, staging_buffer_memory);

    create_image(
        physical_device,
        device,
        width,
        height,
        mip_levels,
        msaa_sample_count,
        VK_FORMAT_R8G8B8A8_SRGB,
        VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        texture_image,
        texture_image_memory
    );

    set_debug_name(
        device,
        VK_OBJECT_TYPE_IMAGE,
        texture_image,
        (filename + "_image").c_str()
    );

    VkCommandBuffer command_buffer = begin_single_time_commands(device, command_pool);

    //  Prepare texture for staging buffer copy
    record_transition_image_layout_commands(
        command_buffer,
        texture_image,
        VK_FORMAT_R8G8B8A8_SRGB,
        mip_levels,
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
    );

    copy_buffer_to_image(
        command_buffer,
        staging_buffer,
        texture_image,
        static_cast<uint32_t>(width),
        static_cast<uint32_t>(height)
    );

    //  Prepare texture for shader access
    // transition_image_layout(
    //     device,
    //     transfer_queue,
    //     command_pool,
    //     texture_image,
    //     VK_FORMAT_R8G8B8A8_SRGB,
    //     mip_levels,
    //     VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
    //     VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
    // );

    generate_mipmaps(
        command_buffer,
        texture_image,
        width,
        height,
        mip_levels
    );

    transfer_queue.end_single_time_commands(command_pool, command_buffer);

    vkDestroyBuffer(device, staging_buffer, nullptr);
    vkFreeMemory(device, staging_buffer_memory, nullptr);
}

//  ----------------------------------------------------------------------------
static void create_texture_image_view(
    VkDevice device,
    uint32_t mip_levels,
    VkImage& texture_image,
    VkImageView& texture_image_view
) {
    texture_image_view = create_image_view(
        device,
        texture_image,
        VK_FORMAT_R8G8B8A8_SRGB,
        mip_levels,
        VK_IMAGE_ASPECT_COLOR_BIT
    );
}

//  ----------------------------------------------------------------------------
static void create_texture_sampler(
    VkDevice device,
    uint32_t mipmap_levels,
    VkSampler& texture_sampler
) {
    VkSamplerCreateInfo sampler_info{};
    sampler_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    sampler_info.magFilter = VK_FILTER_LINEAR;
    sampler_info.minFilter = VK_FILTER_LINEAR;

    sampler_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sampler_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sampler_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

    sampler_info.anisotropyEnable = VK_TRUE;
    sampler_info.maxAnisotropy = 16.0f;

    sampler_info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    sampler_info.unnormalizedCoordinates = VK_FALSE;

    sampler_info.compareEnable = VK_FALSE;
    sampler_info.compareOp = VK_COMPARE_OP_ALWAYS;

    sampler_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    sampler_info.mipLodBias = 0.0f;
    sampler_info.minLod = 0.0f;
    sampler_info.maxLod = static_cast<float>(mipmap_levels);

    if (vkCreateSampler(device, &sampler_info, nullptr, &texture_sampler) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create texture sampler.");
    }
}

//  ----------------------------------------------------------------------------
void create_texture(
    VkPhysicalDevice physical_device,
    VkDevice device,
    VulkanQueue& transfer_queue,
    VkCommandPool command_pool,
    const std::string& filename,
    Texture& texture
) {
    create_texture_image(
        physical_device,
        device,
        transfer_queue,
        command_pool,
        VK_SAMPLE_COUNT_1_BIT,
        filename,
        texture.image,
        texture.mip_levels,
        texture.image_memory
    );

    create_texture_image_view(device, texture.mip_levels, texture.image, texture.view);
    set_debug_name(
        device,
        VK_OBJECT_TYPE_IMAGE_VIEW,
        texture.view,
        (filename + "_image_view").c_str()
    );

    create_texture_sampler(device, texture.mip_levels, texture.sampler);
    set_debug_name(
        device,
        VK_OBJECT_TYPE_SAMPLER,
        texture.sampler,
        (filename + "_sampler").c_str()
    );

}

//  ----------------------------------------------------------------------------
void destroy_texture(VkDevice device, const Texture& texture) {
    //  Destroy texture sampler
    vkDestroySampler(device, texture.sampler, nullptr);

    //  Destroy texture image view
    vkDestroyImageView(device, texture.view, nullptr);

    //  Destroy texture
    vkDestroyImage(device, texture.image, nullptr);
    vkFreeMemory(device, texture.image_memory, nullptr);
}
}
