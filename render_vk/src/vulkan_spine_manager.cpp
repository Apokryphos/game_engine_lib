#include "assets/spine_asset.hpp"
#include "render_vk/spine_model.hpp"
#include "render_vk/vulkan_spine_manager.hpp"

using namespace assets;

namespace render_vk
{
//  ----------------------------------------------------------------------------
static void unload_spine_model(std::unique_ptr<SpineModel>& model) {
    model->anim_state_data = nullptr;
    model->skeleton = nullptr;
    model->atlas = nullptr;
    model->skeleton_data = nullptr;
    model->texture_loader = nullptr;
    model->model.unload();
}

//  ----------------------------------------------------------------------------
VulkanSpineManager::VulkanSpineManager() {
}

//  ----------------------------------------------------------------------------
VulkanSpineManager::~VulkanSpineManager() {
}

//  ----------------------------------------------------------------------------
void VulkanSpineManager::add_spine_model(std::unique_ptr<SpineModel> model) {
    std::lock_guard<std::mutex> lock(m_mutex);

    //  Create asset
    SpineAsset asset {};
    asset.id = model->model.get_id();
    asset.anim_state_data = model->anim_state_data.get();
    asset.atlas = model->atlas.get();
    asset.skeleton = model->skeleton.get();
    asset.skeleton_data = model->skeleton_data;

    //  Add asset
    m_assets[asset.id] = asset;

    //  Add model to active set
    m_added.push_back(std::move(model));
}

//  ----------------------------------------------------------------------------
const SpineAsset* VulkanSpineManager::get_asset(const AssetId id) const {
    std::lock_guard<std::mutex> lock(m_mutex);

    const auto asset_itr = m_assets.find(id);
    if (asset_itr != m_assets.end()) {
        return &(asset_itr->second);
    }
    return nullptr;
}

//  ----------------------------------------------------------------------------
SpineModel* VulkanSpineManager::get_spine_model(const AssetId id) const {
    std::lock_guard<std::mutex> lock(m_mutex);

    auto find = m_models.find(id);

    if (find == m_models.end()) {
        return nullptr;
    }

    return m_models.at(id).get();
}

//  ----------------------------------------------------------------------------
bool VulkanSpineManager::spine_model_exists(const AssetId id) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_models.find(id) != m_models.end();
}

//  ----------------------------------------------------------------------------
void VulkanSpineManager::unload() {
    for (auto& pair : m_models) {
        unload_spine_model(pair.second);
    }
    m_models.clear();

    for (auto& model : m_added) {
        unload_spine_model(model);
    }
    m_added.clear();
}

//  ----------------------------------------------------------------------------
void VulkanSpineManager::update_models() {
    std::lock_guard<std::mutex> lock(m_mutex);

    for (auto& model : m_added) {
        m_models[model->model.get_id()] = std::move(model);
    }
    m_added.clear();
}
}
