#pragma once

#include "assets/asset_id.hpp"
#include "assets/spine_asset.hpp"
#include "assets/spine_manager.hpp"
#include "render_vk/vulkan.hpp"
#include <map>
#include <memory>
#include <mutex>
#include <vector>

namespace render_vk
{
class SpineModel;
class VulkanQueue;

class VulkanSpineManager : public assets::SpineManager
{
    using AssetId = assets::AssetId;

    mutable std::mutex m_mutex;
    std::map<assets::AssetId, assets::SpineAsset> m_assets;
    std::map<assets::AssetId, std::unique_ptr<SpineModel>> m_models;
    std::vector<std::unique_ptr<SpineModel>> m_added;

public:
    VulkanSpineManager();
    ~VulkanSpineManager();
    VulkanSpineManager(const VulkanSpineManager&) = delete;
    VulkanSpineManager& operator=(const VulkanSpineManager&) = delete;
    void add_spine_model(std::unique_ptr<SpineModel> model);
    virtual const assets::SpineAsset* get_asset(const assets::AssetId id) const override;
    SpineModel* get_spine_model(const AssetId id) const;
    bool spine_model_exists(const AssetId id) const;
    void unload();
    void update_models();
};
}
