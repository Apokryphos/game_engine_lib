#include "assets/asset_id.hpp"
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
    assert(model != nullptr);
    m_added.push_back(std::move(model));
}

//  ----------------------------------------------------------------------------
const VulkanModel& ModelManager::get_glyph_quad() const {
    return *m_glyph_quad;
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

    const float SPRITE_SIZE = 0.5f;

    sprite_mesh.vertices = {
        {
            { -SPRITE_SIZE, -SPRITE_SIZE, 0.0f },
            {  1.0f,  1.0f, 1.0f },
            {  0.0f,  0.0f, },
        },
        {
            {  SPRITE_SIZE, -SPRITE_SIZE, 0.0f },
            {  1.0f,  1.0f, 1.0f },
            {  1.0f,  0.0f, },
        },
        {
            {  SPRITE_SIZE,  SPRITE_SIZE, 0.0f },
            {  1.0f,  1.0f, 1.0f },
            {  1.0f,  1.0f, },
        },
        {
            { -SPRITE_SIZE,  SPRITE_SIZE, 0.0f },
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

    //  Glyph quad
    Mesh glyph_mesh;

    glyph_mesh.vertices = {
        {
            { 0.0f, 0.0f, 0.0f },
            { 1.0f, 1.0f, 1.0f },
            { 0.0f, 0.0f, },
        },
        {
            { 1.0f, 0.0f, 0.0f },
            { 1.0f, 1.0f, 1.0f },
            { 1.0f, 0.0f, },
        },
        {
            { 1.0f, 1.0f, 0.0f },
            { 1.0f, 1.0f, 1.0f },
            { 1.0f, 1.0f, },
        },
        {
            { 0.0f, 1.0f, 0.0f },
            { 1.0f, 1.0f, 1.0f },
            { 0.0f, 1.0f, },
        }
    };

    //  Counter-clockwise order
    glyph_mesh.indices = { 0, 3, 1, 1, 3, 2 };

    m_glyph_quad = std::make_unique<VulkanModel>(0);

    m_glyph_quad->load(
        physical_device,
        device,
        graphics_queue,
        command_pool,
        glyph_mesh
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

    add_model(std::move(model));

    log_debug("Loaded model '%s' (%d).", path.c_str(), id);
}

//  ----------------------------------------------------------------------------
const VulkanModel& ModelManager::get_billboard_quad() const {
    return *m_billboard_quad;
}

//  ----------------------------------------------------------------------------
VulkanModel* ModelManager::get_model(const AssetId id) const {
    std::lock_guard<std::mutex> lock(m_models_mutex);

    auto find = m_models.find(id);

    if (find == m_models.end()) {
        return nullptr;
    }

    return m_models.at(id).get();
}

//  ----------------------------------------------------------------------------
const VulkanModel& ModelManager::get_sprite_quad() const {
    return *m_sprite_quad;
}

//  ----------------------------------------------------------------------------
bool ModelManager::model_exists(const AssetId id) const {
    std::lock_guard<std::mutex> lock(m_models_mutex);
    return m_models.find(id) != m_models.end();
}

//  ----------------------------------------------------------------------------
void ModelManager::update_models() {
    std::lock_guard<std::mutex> lock(m_models_mutex);

    for (auto& model : m_added) {
        m_models[model->get_id()] = std::move(model);
    }
    m_added.clear();
}

//  ----------------------------------------------------------------------------
void ModelManager::unload(VkDevice device) {
    m_billboard_quad->unload();
    m_billboard_quad = nullptr;

    m_glyph_quad->unload();
    m_glyph_quad = nullptr;

    m_sprite_quad->unload();
    m_sprite_quad = nullptr;

    for (auto& pair : m_models) {
        pair.second->unload();
    }
    m_models.clear();

    for (auto& model : m_added) {
        model->unload();
    }
    m_added.clear();
}
}
