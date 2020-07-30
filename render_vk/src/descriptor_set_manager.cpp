#include "render_vk/debug_utils.hpp"
#include "render_vk/descriptor_set_manager.hpp"
#include "render_vk/texture_manager.hpp"
#include <mutex>

namespace render_vk
{
//  ----------------------------------------------------------------------------
static void create_descriptor_pool(
    VkDevice device,
    VkDescriptorPool& descriptor_pool
) {
    const uint32_t sampler_count = MAX_TEXTURES;
    const uint32_t max_sets = 2 + (sampler_count);

    std::array<VkDescriptorPoolSize, 2> pool_sizes{};
    pool_sizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    pool_sizes[0].descriptorCount = 1;
    pool_sizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    pool_sizes[1].descriptorCount = sampler_count;

    VkDescriptorPoolCreateInfo pool_info{};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.poolSizeCount = static_cast<uint32_t>(pool_sizes.size());
    pool_info.pPoolSizes = pool_sizes.data();
    pool_info.maxSets = max_sets;

    if (vkCreateDescriptorPool(device, &pool_info, nullptr, &descriptor_pool) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create descriptor pool.");
    }
}

//  ----------------------------------------------------------------------------
static void create_descriptor_set(
    const VkDevice device,
    const VkDescriptorSetLayout descriptor_set_layout,
    const VkDescriptorPool descriptor_pool,
    VkDescriptorSet& descriptor_set
) {
    VkDescriptorSetAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    alloc_info.descriptorPool = descriptor_pool;
    alloc_info.descriptorSetCount = 1;
    alloc_info.pSetLayouts = &descriptor_set_layout;

    if (vkAllocateDescriptorSets(device, &alloc_info, &descriptor_set) != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate descriptor set.");
    }

    set_debug_name(
        device,
        VK_OBJECT_TYPE_DESCRIPTOR_SET,
        descriptor_set,
        "texture_template_descriptor_set"
    );
}

//  ----------------------------------------------------------------------------
static void update_texture_descriptor_sets(
    VkDevice device,
    const std::vector<Texture>& textures,
    VkDescriptorSet& descriptor_set
) {
    std::vector<VkDescriptorImageInfo> image_infos(textures.size());
    for (size_t n = 0; n < image_infos.size(); ++n) {
        image_infos[n].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        image_infos[n].imageView = textures[n].view;
        image_infos[n].sampler = textures[n].sampler;
    }

    std::array<VkWriteDescriptorSet, 1> descriptor_writes{};

    //  Combined texture sampler
    descriptor_writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptor_writes[0].dstSet = descriptor_set;
    descriptor_writes[0].dstBinding = 0;
    descriptor_writes[0].dstArrayElement = 0;
    descriptor_writes[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptor_writes[0].descriptorCount = static_cast<uint32_t>(image_infos.size());
    descriptor_writes[0].pBufferInfo = nullptr;
    descriptor_writes[0].pImageInfo = image_infos.data();
    descriptor_writes[0].pTexelBufferView = nullptr;

    vkUpdateDescriptorSets(
        device,
        static_cast<uint32_t>(descriptor_writes.size()),
        descriptor_writes.data(),
        0,
        nullptr
    );
}

//  ----------------------------------------------------------------------------
void DescriptorSetManager::copy_texture_descriptor_set(VkDescriptorSet dst) {
    std::lock_guard<std::mutex> lock(m_mutex);

    VkCopyDescriptorSet copy{};
    copy.sType = VK_STRUCTURE_TYPE_COPY_DESCRIPTOR_SET;
    copy.descriptorCount = m_texture_descriptors_count;
    copy.dstSet = dst;
    copy.srcSet = m_texture_descriptor_set;

    vkUpdateDescriptorSets(
        m_device,
        0,
        nullptr,
        1,
        &copy
    );
}

//  ----------------------------------------------------------------------------
DescriptorSetManager::DescriptorSetManager(
    VkDevice device,
    VkDescriptorSetLayout texture_descriptor_set_layout
) : m_device(device) {
    create_descriptor_pool(m_device, m_descriptor_pool);

    create_descriptor_set(
        device,
        texture_descriptor_set_layout,
        m_descriptor_pool,
        m_texture_descriptor_set
    );
}

//  ----------------------------------------------------------------------------
void DescriptorSetManager::destroy() {
    vkDestroyDescriptorPool(m_device, m_descriptor_pool, nullptr);
}

//  ----------------------------------------------------------------------------
void DescriptorSetManager::update_descriptor_sets(TextureManager& texture_mgr) {
    std::lock_guard<std::mutex> lock(m_mutex);

    uint32_t texture_timestamp = texture_mgr.get_timestamp();
    if (m_texture_timestamp == texture_timestamp) {
        return;
    }

    m_texture_timestamp = texture_timestamp;

    std::vector<Texture> textures;
    texture_mgr.get_textures(textures);
    update_texture_descriptor_sets(m_device, textures, m_texture_descriptor_set);
}
}
