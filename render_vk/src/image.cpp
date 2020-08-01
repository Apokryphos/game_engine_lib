#include "render_vk/command_buffer.hpp"
#include "render_vk/depth.hpp"
#include "render_vk/vulkan.hpp"
#include "render_vk/vulkan_queue.hpp"

namespace render_vk
{
//  ----------------------------------------------------------------------------
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
) {
    VkImageCreateInfo image_info{};
    image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    image_info.imageType = VK_IMAGE_TYPE_2D;
    image_info.extent.width = width;
    image_info.extent.height = height;
    image_info.extent.depth = 1;
    image_info.mipLevels = mip_levels;
    image_info.arrayLayers = 1;
    image_info.format = format;
    image_info.tiling = tiling;
    image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    image_info.usage = usage;
    image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    image_info.samples = VK_SAMPLE_COUNT_1_BIT;
    image_info.flags = 0; // Optional

    if (vkCreateImage(device, &image_info, nullptr, &image) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create image.");
    }

    VkMemoryRequirements mem_requirements;
    vkGetImageMemoryRequirements(device, image, &mem_requirements);

    VkMemoryAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    alloc_info.allocationSize = mem_requirements.size;
    alloc_info.memoryTypeIndex = find_memory_type(
        physical_device,
        mem_requirements.memoryTypeBits,
        properties
    );

    if (vkAllocateMemory(device, &alloc_info, nullptr, &image_memory) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate image memory!");
    }

    vkBindImageMemory(device, image, image_memory, 0);
}

//  ----------------------------------------------------------------------------
void record_transition_image_layout_commands(
    VkCommandBuffer command_buffer,
    VkImage image,
    VkFormat format,
    uint32_t mip_levels,
    VkImageLayout old_layout,
    VkImageLayout new_layout
) {
    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = old_layout;
    barrier.newLayout = new_layout;

    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

    barrier.image = image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = mip_levels;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    VkPipelineStageFlags src_stage;
    VkPipelineStageFlags dst_stage;

    //  Subresource aspect
    if (new_layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

        if (has_stencil_component(format)) {
            barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
        }
    } else {
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    }

    //  Access masks and pipeline stages
    if (old_layout == VK_IMAGE_LAYOUT_UNDEFINED &&
        new_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
    ) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        src_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        dst_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    } else if (
        old_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
        new_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
    ) {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        src_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        dst_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    } else if (
        old_layout == VK_IMAGE_LAYOUT_UNDEFINED &&
        new_layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
    ) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask =
            VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
            VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

            src_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            dst_stage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    } else {
        throw std::invalid_argument("Unsupported layout transition.");
    }

    vkCmdPipelineBarrier(
        command_buffer,
        src_stage,
        dst_stage,
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
void transition_image_layout(
    VkDevice device,
    VulkanQueue& transfer_queue,
    VkCommandPool command_pool,
    VkImage image,
    VkFormat format,
    uint32_t mip_levels,
    VkImageLayout old_layout,
    VkImageLayout new_layout
) {
    VkCommandBuffer command_buffer = begin_single_time_commands(
        device,
        command_pool
    );

    record_transition_image_layout_commands(
        command_buffer,
        image,
        format,
        mip_levels,
        old_layout,
        new_layout
    );

    transfer_queue.end_single_time_commands(command_pool, command_buffer);
}
}
