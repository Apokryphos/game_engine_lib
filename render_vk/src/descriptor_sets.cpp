#include "render_vk/vulkan.hpp"
#include "render_vk/vulkan_swapchain.hpp"
#include <array>
#include <stdexcept>
#include <vector>

namespace render_vk
{
//  ----------------------------------------------------------------------------
void create_descriptor_sets(
    VkDevice device,
    const VulkanSwapchain swapchain,
    VkDescriptorSetLayout descriptor_set_layout,
    VkDescriptorPool descriptor_pool,
    std::vector<VkDescriptorSet>& descriptor_sets
) {
    const size_t image_count = swapchain.images.size();

    std::vector<VkDescriptorSetLayout> layouts(image_count, descriptor_set_layout);
    VkDescriptorSetAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    alloc_info.descriptorPool = descriptor_pool;
    alloc_info.descriptorSetCount = static_cast<uint32_t>(image_count);
    alloc_info.pSetLayouts = layouts.data();

    descriptor_sets.resize(image_count);
    if (vkAllocateDescriptorSets(device, &alloc_info, descriptor_sets.data()) != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate descriptor sets.");
    }
}

//  ----------------------------------------------------------------------------
void update_descriptor_sets(
    VkDevice device,
    const VulkanSwapchain swapchain,
    VkImageView texture_image_view,
    VkSampler texture_sampler,
    VkBuffer frame_uniform_buffer,
    VkBuffer object_uniform_buffer,
    size_t frame_ubo_size,
    std::vector<VkDescriptorSet>& descriptor_sets
) {
    const size_t image_count = swapchain.images.size();

    for (size_t n = 0; n < image_count; n++) {
        VkDescriptorBufferInfo frame_buffer_info{};
        frame_buffer_info.buffer = frame_uniform_buffer;
        frame_buffer_info.offset = 0;
        frame_buffer_info.range = frame_ubo_size;

        VkDescriptorBufferInfo object_buffer_info{};
        object_buffer_info.buffer = object_uniform_buffer;
        object_buffer_info.offset = 0;
        object_buffer_info.range = VK_WHOLE_SIZE;

        VkDescriptorImageInfo image_info{};
        image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        image_info.imageView = texture_image_view;
        image_info.sampler = texture_sampler;

        std::array<VkWriteDescriptorSet, 3> descriptor_writes{};

        //  Per-frame dynamic uniform buffer
        descriptor_writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptor_writes[0].dstSet = descriptor_sets[n];
        descriptor_writes[0].dstBinding = 0;
        descriptor_writes[0].dstArrayElement = 0;
        descriptor_writes[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptor_writes[0].descriptorCount = 1;
        descriptor_writes[0].pBufferInfo = &frame_buffer_info;
        descriptor_writes[0].pImageInfo = nullptr;
        descriptor_writes[0].pTexelBufferView = nullptr;

        //  Texture sampler
        descriptor_writes[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptor_writes[1].dstSet = descriptor_sets[n];
        descriptor_writes[1].dstBinding = 1;
        descriptor_writes[1].dstArrayElement = 0;
        descriptor_writes[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptor_writes[1].descriptorCount = 1;
        descriptor_writes[1].pBufferInfo = nullptr;
        descriptor_writes[1].pImageInfo = &image_info;
        descriptor_writes[1].pTexelBufferView = nullptr;

        //  Per-object dynamic uniform buffer
        descriptor_writes[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptor_writes[2].dstSet = descriptor_sets[n];
        descriptor_writes[2].dstBinding = 2;
        descriptor_writes[2].dstArrayElement = 0;
        descriptor_writes[2].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
        descriptor_writes[2].descriptorCount = 1;
        descriptor_writes[2].pBufferInfo = &object_buffer_info;
        descriptor_writes[2].pImageInfo = nullptr;
        descriptor_writes[2].pTexelBufferView = nullptr;

        vkUpdateDescriptorSets(
            device,
            static_cast<uint32_t>(descriptor_writes.size()),
            descriptor_writes.data(),
            0,
            nullptr
        );
    }
}
}
