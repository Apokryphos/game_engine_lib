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
ModelManager::ModelManager() {
}

//  ----------------------------------------------------------------------------
ModelManager::~ModelManager() {
    // unload();
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
    return m_models.at(id).get();
}

//  ----------------------------------------------------------------------------
void ModelManager::unload() {
    m_models.clear();
}
}
