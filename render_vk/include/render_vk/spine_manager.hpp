#pragma once

#include "assets/asset_id.hpp"
#include "render_vk/vulkan.hpp"
#include <map>
#include <memory>
#include <mutex>
#include <vector>

namespace render_vk
{
class SpineModel;
class VulkanQueue;

class SpineManager
{
    using AssetId = assets::AssetId;

    mutable std::mutex m_mutex;
    std::map<assets::AssetId, std::unique_ptr<SpineModel>> m_models;
    std::vector<std::unique_ptr<SpineModel>> m_added;

public:
    SpineManager();
    ~SpineManager();
    SpineManager(const SpineManager&) = delete;
    SpineManager& operator=(const SpineManager&) = delete;
    void add_spine_model(std::unique_ptr<SpineModel> model);
    SpineModel* get_spine_model(const AssetId id) const;
    bool spine_model_exists(const AssetId id) const;
    void unload();
    void update_models();
};
}
