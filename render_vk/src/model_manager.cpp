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
void ModelManager::add_model(std::unique_ptr<VulkanModel> model) {
    std::lock_guard<std::mutex> lock(m_models_mutex);

    m_models[model->get_id()] = std::move(model);
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
void ModelManager::initialize(
    VkPhysicalDevice physical_device,
    VkDevice device,
    VkQueue graphics_queue,
    VkCommandPool command_pool
) {
    Mesh mesh;

    mesh.vertices = {
        {
            { -1.0f, -1.0f, 0.0f },
            {  1.0f,  1.0f, 1.0f },
            {  0.0f,  0.0f, },
        },
        {
            {  1.0f, -1.0f, 0.0f },
            {  1.0f,  1.0f, 1.0f },
            {  0.0f,  0.0f, },
        },
        {
            {  1.0f,  1.0f, 0.0f },
            {  1.0f,  1.0f, 1.0f },
            {  0.0f,  0.0f, },
        },
        {
            { -1.0f,  1.0f, 0.0f },
            {  1.0f,  1.0f, 1.0f },
            {  0.0f,  0.0f, },
        }
    };

    mesh.indices = { 0, 1, 3, 1, 2, 3 };

    m_quad = std::make_unique<VulkanModel>(0);

    m_quad->load(
        physical_device,
        device,
        graphics_queue,
        command_pool,
        mesh
    );
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

    auto model = std::make_unique<VulkanModel>(id);
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
VulkanModel& ModelManager::get_quad() {
    return *m_quad;
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
void ModelManager::unload(VkDevice device) {
    m_quad->unload();
    m_quad = nullptr;

    for (auto& pair : m_models) {
        pair.second->unload();
    }
    m_models.clear();

    for (auto& pair : m_textures) {
        destroy_texture(device, pair.second);
    }
}
}
