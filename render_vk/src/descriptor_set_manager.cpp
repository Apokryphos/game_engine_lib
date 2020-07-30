#include "render_vk/debug_utils.hpp"
#include "render_vk/descriptor_set_manager.hpp"
#include "render_vk/texture_manager.hpp"
#include <mutex>

namespace render_vk
{
//  ----------------------------------------------------------------------------
static void update_texture_descriptor_sets(
    VkDevice device,
    const std::vector<Texture>& textures,
    VkDescriptorSet& descriptor_set
) {
    assert(!textures.empty());

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
DescriptorSetManager::DescriptorSetManager(
    VkDevice device,
    TextureManager& texture_mgr
) : m_device(device),
    m_texture_mgr(texture_mgr) {
}

//  ----------------------------------------------------------------------------
void DescriptorSetManager::copy_texture_descriptor_set(VkDescriptorSet dst) {
    std::lock_guard<std::mutex> lock(m_mutex);

    assert(!m_textures.empty());

    update_texture_descriptor_sets(m_device, m_textures, dst);
}

//  ----------------------------------------------------------------------------
bool DescriptorSetManager::is_ready() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return !m_textures.empty();
}

//  ----------------------------------------------------------------------------
void DescriptorSetManager::update_descriptor_sets(TextureManager& texture_mgr) {
    std::lock_guard<std::mutex> lock(m_mutex);

    const uint32_t texture_timestamp = texture_mgr.get_timestamp();
    if (m_texture_timestamp == texture_timestamp) {
        return;
    }

    m_texture_timestamp = texture_timestamp;

    texture_mgr.get_textures(m_textures);
}
}
