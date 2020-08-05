#include "render_vk/spine_manager.hpp"
#include "render_vk/spine_model.hpp"

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
SpineManager::SpineManager() {
}

//  ----------------------------------------------------------------------------
SpineManager::~SpineManager() {
}

//  ----------------------------------------------------------------------------
void SpineManager::add_spine_model(std::unique_ptr<SpineModel> model) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_added.push_back(std::move(model));
}

//  ----------------------------------------------------------------------------
SpineModel* SpineManager::get_spine_model(const AssetId id) const {
    std::lock_guard<std::mutex> lock(m_mutex);

    auto find = m_models.find(id);

    if (find == m_models.end()) {
        return nullptr;
    }

    return m_models.at(id).get();
}

//  ----------------------------------------------------------------------------
bool SpineManager::spine_model_exists(const AssetId id) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_models.find(id) != m_models.end();
}

//  ----------------------------------------------------------------------------
void SpineManager::unload() {
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
void SpineManager::update_models() {
    std::lock_guard<std::mutex> lock(m_mutex);

    for (auto& model : m_added) {
        m_models[model->model.get_id()] = std::move(model);
    }
    m_added.clear();
}
}
