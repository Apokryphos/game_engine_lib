#include "common/asset.hpp"
#include "render_vk/mesh.hpp"
#include "render_vk/model_manager.hpp"
#include "render_vk/vulkan_model.hpp"
#include <map>
#include <memory>

using namespace common;

namespace render_vk
{
//  ----------------------------------------------------------------------------
ModelManager::ModelManager()
: m_rebuild_descriptor_sets(false) {
}

//  ----------------------------------------------------------------------------
ModelManager::~ModelManager() {
    // unload();
}

//  ----------------------------------------------------------------------------
void ModelManager::add_model(
    const AssetId id,
    std::unique_ptr<VulkanModel> model
) {
    std::lock_guard<std::mutex> lock(m_models_mutex);

    m_models[id] = std::move(model);
}

//  ----------------------------------------------------------------------------
void ModelManager::add_texture(
    const AssetId id,
    const Texture& texture
) {
    std::lock_guard<std::mutex> lock(m_textures_mutex);

    m_textures[id] = texture;
    m_rebuild_descriptor_sets = true;
}

//  ----------------------------------------------------------------------------
void ModelManager::load_model(
    const AssetId id,
    const std::string& path,
    VkPhysicalDevice physical_device,
    VkDevice device,
    VkQueue graphics_queue,
    VkCommandPool command_pool
) {
    Mesh mesh;
    load_mesh(mesh, path);

    auto model = std::make_unique<VulkanModel>(id, path);
    model->load(
        physical_device,
        device,
        graphics_queue,
        command_pool,
        mesh
    );

    m_models[id] = std::move(model);
}

//  ----------------------------------------------------------------------------
VulkanModel* ModelManager::get_model(const AssetId id) {
    std::lock_guard<std::mutex> lock(m_models_mutex);

    auto find = m_models.find(id);

    if (find == m_models.end()) {
        return nullptr;
    }

    return m_models.at(id).get();
}

//  ----------------------------------------------------------------------------
void ModelManager::get_textures(std::vector<Texture>& textures) {
    textures.clear();

    std::lock_guard<std::mutex> lock(m_textures_mutex);
    for (const auto& pair : m_textures) {
        textures.push_back(pair.second);
    }
}

//  ----------------------------------------------------------------------------
void ModelManager::unload() {
    m_models.clear();
}
}
