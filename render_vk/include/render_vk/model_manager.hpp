#pragma once

#include "common/asset.hpp"
#include "render_vk/texture.hpp"
#include "render_vk/vulkan.hpp"
#include <map>
#include <memory>
#include <mutex>

namespace render_vk
{
class VulkanModel;

class ModelManager
{
    using AssetId = common::AssetId;

    bool m_rebuild_descriptor_sets;
    std::mutex m_models_mutex;
    std::mutex m_textures_mutex;
    std::map<common::AssetId, std::unique_ptr<VulkanModel>> m_models;
    std::map<common::AssetId, Texture> m_textures;

public:
    ModelManager();
    ~ModelManager();
    ModelManager(const ModelManager&) = delete;
    ModelManager& operator=(const ModelManager&) = delete;

    void add_model(const AssetId id, std::unique_ptr<VulkanModel> model);
    void add_texture(const AssetId id, const Texture& texture);

    bool descriptor_sets_changed() {
        const bool changed = m_rebuild_descriptor_sets;
        m_rebuild_descriptor_sets = false;
        return changed;
    }

    void load_model(
        const AssetId id,
        const std::string& path,
        VkPhysicalDevice physical_device,
        VkDevice device,
        VkQueue graphics_queue,
        VkCommandPool command_pool
    );

    VulkanModel* get_model(const AssetId id);
    void get_textures(std::vector<Texture>& textures);
    void unload();
};
}
