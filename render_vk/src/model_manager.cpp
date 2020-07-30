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
void ModelManager::add_model(std::unique_ptr<VulkanModel> model) {
    std::lock_guard<std::mutex> lock(m_models_mutex);

    m_models[model->get_id()] = std::move(model);
}

//  ----------------------------------------------------------------------------
void ModelManager::initialize(
    VkPhysicalDevice physical_device,
    VkDevice device,
    VulkanQueue& graphics_queue,
    VkCommandPool command_pool
) {
    //  Billboard quad
    Mesh billboard_mesh;

    billboard_mesh.vertices = {
        {
            { -1.0f,  0.0f, 1.0f },
            {  1.0f,  1.0f, 1.0f },
            {  0.0f,  0.0f, },
        },
        {
            {  1.0f,  0.0f, 1.0f },
            {  1.0f,  1.0f, 1.0f },
            {  1.0f,  0.0f, },
        },
        {
            {  1.0f,  0.0f, 0.0f },
            {  1.0f,  1.0f, 1.0f },
            {  1.0f,  1.0f, },
        },
        {
            { -1.0f,  0.0f, 0.0f },
            {  1.0f,  1.0f, 1.0f },
            {  0.0f,  1.0f, },
        }
    };

    //  Counter-clockwise order
    billboard_mesh.indices = { 0, 3, 1, 1, 3, 2 };

    m_billboard_quad = std::make_unique<VulkanModel>(0);

    m_billboard_quad->load(
        physical_device,
        device,
        graphics_queue,
        command_pool,
        billboard_mesh
    );

    //  Sprite quad
    Mesh sprite_mesh;

    sprite_mesh.vertices = {
        {
            { -1.0f, -1.0f, 0.0f },
            {  1.0f,  1.0f, 1.0f },
            {  0.0f,  0.0f, },
        },
        {
            {  1.0f, -1.0f, 0.0f },
            {  1.0f,  1.0f, 1.0f },
            {  1.0f,  0.0f, },
        },
        {
            {  1.0f,  1.0f, 0.0f },
            {  1.0f,  1.0f, 1.0f },
            {  1.0f,  1.0f, },
        },
        {
            { -1.0f,  1.0f, 0.0f },
            {  1.0f,  1.0f, 1.0f },
            {  0.0f,  1.0f, },
        }
    };

    //  Counter-clockwise order
    sprite_mesh.indices = { 0, 3, 1, 1, 3, 2 };

    m_sprite_quad = std::make_unique<VulkanModel>(0);

    m_sprite_quad->load(
        physical_device,
        device,
        graphics_queue,
        command_pool,
        sprite_mesh
    );
}

//  ----------------------------------------------------------------------------
void ModelManager::load_model(
    const AssetId id,
    const std::string& path,
    VkPhysicalDevice physical_device,
    VkDevice device,
    VulkanQueue& graphics_queue,
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
VulkanModel& ModelManager::get_billboard_quad() {
    return *m_billboard_quad;
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
VulkanModel& ModelManager::get_sprite_quad() {
    return *m_sprite_quad;
}

//  ----------------------------------------------------------------------------
void ModelManager::unload(VkDevice device) {
    m_billboard_quad->unload();
    m_billboard_quad = nullptr;

    m_sprite_quad->unload();
    m_sprite_quad = nullptr;

    for (auto& pair : m_models) {
        pair.second->unload();
    }
    m_models.clear();
}
}
