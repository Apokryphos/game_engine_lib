#include "render_vk/descriptor_sets.hpp"
#include "render_vk/descriptor_set_layout.hpp"
#include "render_vk/texture.hpp"
#include "render_vk/vulkan.hpp"
#include <array>
#include <stdexcept>
#include <vector>

namespace render_vk
{
//  ----------------------------------------------------------------------------
static void create_descriptor_sets(
    VkDevice device,
    const uint32_t swapchain_image_count,
    VkDescriptorSetLayout descriptor_set_layout,
    VkDescriptorPool descriptor_pool,
    std::vector<VkDescriptorSet>& descriptor_sets
) {
    std::vector<VkDescriptorSetLayout> layouts(swapchain_image_count, descriptor_set_layout);
    VkDescriptorSetAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    alloc_info.descriptorPool = descriptor_pool;
    alloc_info.descriptorSetCount = static_cast<uint32_t>(swapchain_image_count);
    alloc_info.pSetLayouts = layouts.data();

    descriptor_sets.resize(swapchain_image_count);
    if (vkAllocateDescriptorSets(device, &alloc_info, descriptor_sets.data()) != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate descriptor sets.");
    }
}

//  ----------------------------------------------------------------------------
void create_descriptor_sets(
    VkDevice device,
    const uint32_t swapchain_image_count,
    const DescriptorSetLayouts& descriptor_set_layouts,
    VkDescriptorPool descriptor_pool,
    DescriptorSets& descriptor_sets
) {
    create_descriptor_sets(
        device,
        swapchain_image_count,
        descriptor_set_layouts.frame,
        descriptor_pool,
        descriptor_sets.frame_sets
    );

    create_descriptor_sets(
        device,
        swapchain_image_count,
        descriptor_set_layouts.object,
        descriptor_pool,
        descriptor_sets.object_sets
    );
}

//  ----------------------------------------------------------------------------
void update_frame_descriptor_sets(
    VkDevice device,
    const uint32_t swapchain_image_count,
    VkBuffer frame_uniform_buffer,
    size_t frame_ubo_size,
    std::vector<VkDescriptorSet>& descriptor_sets
) {
    for (size_t n = 0; n < swapchain_image_count; n++) {
        VkDescriptorBufferInfo frame_buffer_info{};
        frame_buffer_info.buffer = frame_uniform_buffer;
        frame_buffer_info.offset = 0;
        frame_buffer_info.range = frame_ubo_size;

        std::array<VkWriteDescriptorSet, 1> descriptor_writes{};

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

        vkUpdateDescriptorSets(
            device,
            static_cast<uint32_t>(descriptor_writes.size()),
            descriptor_writes.data(),
            0,
            nullptr
        );
    }
}

//  ----------------------------------------------------------------------------
void update_object_descriptor_sets(
    VkDevice device,
    const uint32_t swapchain_image_count,
    std::vector<Texture>& textures,
    VkBuffer object_uniform_buffer,
    std::vector<VkDescriptorSet>& descriptor_sets
) {
    for (size_t n = 0; n < swapchain_image_count; n++) {
        VkDescriptorBufferInfo object_buffer_info{};
        object_buffer_info.buffer = object_uniform_buffer;
        object_buffer_info.offset = 0;
        object_buffer_info.range = VK_WHOLE_SIZE;

        std::vector<VkDescriptorImageInfo> image_infos(textures.size());
        for (size_t n = 0; n < image_infos.size(); ++n) {
            image_infos[n].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            image_infos[n].imageView = textures[n].view;
            image_infos[n].sampler = textures[n].sampler;
        }

        std::array<VkWriteDescriptorSet, 2> descriptor_writes{};

        //  Per-object dynamic uniform buffer
        descriptor_writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptor_writes[0].dstSet = descriptor_sets[n];
        descriptor_writes[0].dstBinding = 0;
        descriptor_writes[0].dstArrayElement = 0;
        descriptor_writes[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
        descriptor_writes[0].descriptorCount = 1;
        descriptor_writes[0].pBufferInfo = &object_buffer_info;
        descriptor_writes[0].pImageInfo = nullptr;
        descriptor_writes[0].pTexelBufferView = nullptr;

        //  Combined texture sampler
        descriptor_writes[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptor_writes[1].dstSet = descriptor_sets[n];
        descriptor_writes[1].dstBinding = 1;
        descriptor_writes[1].dstArrayElement = 0;
        descriptor_writes[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptor_writes[1].descriptorCount = static_cast<uint32_t>(image_infos.size());
        descriptor_writes[1].pBufferInfo = nullptr;
        descriptor_writes[1].pImageInfo = image_infos.data();
        descriptor_writes[1].pTexelBufferView = nullptr;

        vkUpdateDescriptorSets(
            device,
            static_cast<uint32_t>(descriptor_writes.size()),
            descriptor_writes.data(),
            0,
            nullptr
        );
    }
}

//  ----------------------------------------------------------------------------
void update_descriptor_sets(
    VkDevice device,
    const uint32_t swapchain_image_count,
    std::vector<Texture>& textures,
    VkBuffer frame_uniform_buffer,
    VkBuffer object_uniform_buffer,
    size_t frame_ubo_size,
    DescriptorSets& descriptor_sets
) {
    update_frame_descriptor_sets(
        device,
        swapchain_image_count,
        frame_uniform_buffer,
        frame_ubo_size,
        descriptor_sets.frame_sets
    );

    update_object_descriptor_sets(
        device,
        swapchain_image_count,
        textures,
        object_uniform_buffer,
        descriptor_sets.object_sets
    );
}

//  ----------------------------------------------------------------------------
// void update_descriptor_sets(
//     VkDevice device,
//     const uint32_t swapchain_image_count,
//     std::vector<Texture>& textures,
//     VkBuffer frame_uniform_buffer,
//     VkBuffer object_uniform_buffer,
//     size_t frame_ubo_size,
//     std::vector<VkDescriptorSet>& descriptor_sets
// ) {
//     for (size_t n = 0; n < swapchain_image_count; n++) {
//         VkDescriptorBufferInfo frame_buffer_info{};
//         frame_buffer_info.buffer = frame_uniform_buffer;
//         frame_buffer_info.offset = 0;
//         frame_buffer_info.range = frame_ubo_size;

//         VkDescriptorBufferInfo object_buffer_info{};
//         object_buffer_info.buffer = object_uniform_buffer;
//         object_buffer_info.offset = 0;
//         object_buffer_info.range = VK_WHOLE_SIZE;

//         std::vector<VkDescriptorImageInfo> image_infos(textures.size());
//         for (size_t n = 0; n < image_infos.size(); ++n) {
//             image_infos[n].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
//             image_infos[n].imageView = textures[n].view;
//             image_infos[n].sampler = textures[n].sampler;
//         }

//         std::array<VkWriteDescriptorSet, 3> descriptor_writes{};

//         //  Per-frame dynamic uniform buffer
//         descriptor_writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
//         descriptor_writes[0].dstSet = descriptor_sets[n];
//         descriptor_writes[0].dstBinding = 0;
//         descriptor_writes[0].dstArrayElement = 0;
//         descriptor_writes[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
//         descriptor_writes[0].descriptorCount = 1;
//         descriptor_writes[0].pBufferInfo = &frame_buffer_info;
//         descriptor_writes[0].pImageInfo = nullptr;
//         descriptor_writes[0].pTexelBufferView = nullptr;

//         //  Per-object dynamic uniform buffer
//         descriptor_writes[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
//         descriptor_writes[1].dstSet = descriptor_sets[n];
//         descriptor_writes[1].dstBinding = 1;
//         descriptor_writes[1].dstArrayElement = 0;
//         descriptor_writes[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
//         descriptor_writes[1].descriptorCount = 1;
//         descriptor_writes[1].pBufferInfo = &object_buffer_info;
//         descriptor_writes[1].pImageInfo = nullptr;
//         descriptor_writes[1].pTexelBufferView = nullptr;

//         //  Combined texture sampler
//         descriptor_writes[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
//         descriptor_writes[2].dstSet = descriptor_sets[n];
//         descriptor_writes[2].dstBinding = 2;
//         descriptor_writes[2].dstArrayElement = 0;
//         descriptor_writes[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
//         descriptor_writes[2].descriptorCount = static_cast<uint32_t>(image_infos.size());
//         descriptor_writes[2].pBufferInfo = nullptr;
//         descriptor_writes[2].pImageInfo = image_infos.data();
//         descriptor_writes[2].pTexelBufferView = nullptr;

//         vkUpdateDescriptorSets(
//             device,
//             static_cast<uint32_t>(descriptor_writes.size()),
//             descriptor_writes.data(),
//             0,
//             nullptr
//         );
//     }
// }
}
