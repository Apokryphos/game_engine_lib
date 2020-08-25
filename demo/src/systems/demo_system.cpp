#include "assets/asset_manager.hpp"
#include "assets/spine_asset.hpp"
#include "assets/spine_manager.hpp"
#include "demo/systems/demo_system.hpp"
#include "engine/engine.hpp"
#include "engine/game.hpp"
#include "engine/system_manager.hpp"
#include "render/frustum.hpp"
#include "render/spine_sprite_batch.hpp"
#include "render/sprite_batch.hpp"
#include "systems/billboard_system.hpp"
#include "systems/model_system.hpp"
#include "systems/glyph_system.hpp"
#include "systems/position_system.hpp"
#include "systems/spine_system.hpp"
#include "systems/sprite_system.hpp"
#include "systems/system_util.hpp"
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include <map>

using namespace assets;
using namespace ecs;
using namespace engine;
using namespace render;
using namespace systems;

namespace demo
{
struct EntitySort
{
    Entity entity;
    //  Squared distance from origin
    float distance;
    glm::vec3 position;
};

//  ----------------------------------------------------------------------------
static void sort_entities(
    const std::vector<Entity>& entities,
    const glm::vec3 origin,
    PositionSystem& pos_sys,
    std::vector<EntitySort>& sorted
) {
    sorted.resize(entities.size());

    //  Populate
    const size_t entity_count = entities.size();
    for (size_t n = 0; n < entity_count; ++n) {
        sorted[n].entity = entities[n];

        const auto pos_cmpnt = pos_sys.get_component(entities[n]);
        sorted[n].position = pos_sys.get_position(pos_cmpnt);

        sorted[n].distance = glm::distance2(
            origin,
            sorted[n].position
        );
    }

    //  Sort by squared distance from origin
    std::sort(
        sorted.begin(),
        sorted.end(),
        [](EntitySort& a, EntitySort& b) {
            return a.distance < b.distance;
        }
    );
}

//  ----------------------------------------------------------------------------
DemoSystem::DemoSystem()
: System(SYSTEM_ID_DEMO, "demo_system") {
}

//  ----------------------------------------------------------------------------
void DemoSystem::batch_billboards(
    Game& game,
    glm::mat4 view,
    glm::mat4 proj,
    std::vector<SpriteBatch>& billboard_batches
) {
    SystemManager& sys_mgr = game.get_system_manager();

    //  Get drawable entities
    const BillboardSystem& billboard_sys = get_billboard_system(sys_mgr);
    std::vector<Entity> entities;
    billboard_sys.get_entities(entities);

    const size_t entity_count = entities.size();

    std::map<uint32_t, SpriteBatch> batches;

    Frustum frustum(proj * view);

    const PositionSystem& pos_sys = get_position_system(sys_mgr);
    for (size_t n = 0; n < entity_count; ++n) {
        //  Get positions
        const auto pos_cmpnt = pos_sys.get_component(entities[n]);
        glm::vec3 position = pos_sys.get_position(pos_cmpnt);

        const auto billboard_cmpnt = billboard_sys.get_component(entities[n]);
        const uint32_t texture_id = billboard_sys.get_texture_id(billboard_cmpnt);
        const glm::vec2 size = billboard_sys.get_size(billboard_cmpnt);

        //  Billboard bounding box
        const glm::vec3 maxp(
            position.x + size.x,
            position.y + size.y,
            1.0f
        );

        const glm::vec3 minp(
            position.x - size.x,
            position.y - size.y,
            0.0f
        );

        //  Skip objects outside frustum
        if (!frustum.is_box_visible(minp, maxp)) {
            continue;
        }

        SpriteBatch& batch = batches[texture_id];
        batch.texture_id = texture_id;
        batch.positions.push_back(position);
        batch.sizes.push_back({size.x, 1.0f, size.y});
    }

    for (const auto& pair : batches) {
        billboard_batches.push_back(pair.second);
    }
}

//  ----------------------------------------------------------------------------
void DemoSystem::batch_glyphs(
    Game& game,
    glm::mat4 view,
    glm::mat4 proj,
    std::vector<GlyphBatch>& glyph_batches
) {
    SystemManager& sys_mgr = game.get_system_manager();

    //  Get drawable entities
    const GlyphSystem& glyph_sys = get_glyph_system(sys_mgr);
    std::vector<Entity> entities;
    glyph_sys.get_entities(entities);

    const size_t entity_count = entities.size();

    std::map<uint32_t, GlyphBatch> batches;

    Frustum frustum(proj * view);

    const PositionSystem& pos_sys = get_position_system(sys_mgr);
    for (size_t n = 0; n < entity_count; ++n) {
        //  Get positions
        const auto pos_cmpnt = pos_sys.get_component(entities[n]);
        glm::vec3 position = pos_sys.get_position(pos_cmpnt);

        const auto glyph_cmpnt = glyph_sys.get_component(entities[n]);
        const uint32_t texture_id = glyph_sys.get_texture_id(glyph_cmpnt);
        const glm::vec2 size = glyph_sys.get_size(glyph_cmpnt);
        const glm::vec4 bg_color = glyph_sys.get_bg_color(glyph_cmpnt);
        const glm::vec4 fg_color = glyph_sys.get_fg_color(glyph_cmpnt);

        //  Billboard bounding box
        const glm::vec3 maxp(
            position.x + size.x,
            position.y + size.y,
            1.0f
        );

        const glm::vec3 minp(
            position.x - size.x,
            position.y - size.y,
            0.0f
        );

        //  Skip objects outside frustum
        if (!frustum.is_box_visible(minp, maxp)) {
            continue;
        }

        GlyphBatch& batch = batches[texture_id];
        batch.texture_id = texture_id;
        batch.positions.push_back(position);
        batch.sizes.push_back({size.x, size.y, 1.0f});
        batch.bg_colors.push_back(bg_color);
        batch.fg_colors.push_back(fg_color);
    }

    for (const auto& pair : batches) {
        glyph_batches.push_back(pair.second);
    }
}

//  ----------------------------------------------------------------------------
void DemoSystem::batch_models(
    Game& game,
    glm::mat4 view,
    glm::mat4 proj,
    std::vector<ModelBatch>& model_batches
) {
    SystemManager& sys_mgr = game.get_system_manager();

    //  Get drawable entities
    const ModelSystem& model_sys = get_model_system(sys_mgr);
    std::vector<Entity> entities;
    model_sys.get_entities(entities);

    const size_t entity_count = entities.size();

    using Key = std::pair<uint32_t, uint32_t>;
    std::map<Key, ModelBatch> batches;

    Frustum frustum(proj * view);

    const PositionSystem& pos_sys = get_position_system(sys_mgr);
    for (size_t n = 0; n < entity_count; ++n) {
        //  Get positions
        const auto pos_cmpnt = pos_sys.get_component(entities[n]);
        const glm::vec3 position = pos_sys.get_position(pos_cmpnt);

        //  Model bounding box
        const float size = 1.0f;
        const glm::vec3 maxp(
            position.x + size,
            position.y + size,
            position.z + size
        );

        const glm::vec3 minp(
            position.x - size,
            position.y - size,
            position.z - size
        );

        //  Skip objects outside frustum
        if (!frustum.is_box_visible(minp, maxp)) {
            continue;
        }

        const auto model_cmpnt = model_sys.get_component(entities[n]);
        const uint32_t model_id = model_sys.get_model_id(model_cmpnt);
        const uint32_t texture_id = model_sys.get_texture_id(model_cmpnt);

        ModelBatch& batch = batches[{model_id, texture_id}];
        batch.model_id = model_id;
        batch.texture_id = texture_id;
        batch.positions.push_back(position);
    }

    for (const auto& pair : batches) {
        model_batches.push_back(pair.second);
    }
}

//  ----------------------------------------------------------------------------
void DemoSystem::batch_spines(
    Game& game,
    glm::mat4 view,
    glm::mat4 proj,
    std::vector<SpineSpriteBatch>& spine_batches
) {
    SystemManager& sys_mgr = game.get_system_manager();

    //  Get drawable entities
    const SpineSystem& spine_sys = get_spine_system(sys_mgr);
    std::vector<Entity> entities;
    spine_sys.get_entities(entities);

    const size_t entity_count = entities.size();

    std::map<uint32_t, SpineSpriteBatch> batches;

    // Frustum frustum(proj * view);

    Engine& engine = game.get_engine();
    AssetManager& asset_mgr = engine.get_asset_manager();
    SpineManager& spine_mgr = asset_mgr.get_spine_manager();

    const PositionSystem& pos_sys = get_position_system(sys_mgr);

    for (size_t n = 0; n < entity_count; ++n) {
        const auto spine_cmpnt = spine_sys.get_component(entities[n]);
        const uint32_t spine_id = spine_sys.get_spine_id(spine_cmpnt);

        //  Check if assets are ready
        const SpineAsset* asset = spine_mgr.get_asset(spine_id);
        if (asset == nullptr) {
            continue;
        }

        //  Get positions
        const auto pos_cmpnt = pos_sys.get_component(entities[n]);
        glm::vec3 position = pos_sys.get_position(pos_cmpnt);

        //  Sprite bounding box
        // const glm::vec3 maxp(
        //     position.x + size.x,
        //     position.y + size.y,
        //     1.0f
        // );

        // const glm::vec3 minp(
        //     position.x - size.x,
        //     position.y - size.y,
        //     0.0f
        // );

        //  Skip objects outside frustum
        // if (!frustum.is_box_visible(minp, maxp)) {
        //     continue;
        // }

        SpineSpriteBatch& batch = batches[spine_id];
        batch.spine_id = spine_id;
        batch.texture_id = asset->texture_id;
        batch.positions.push_back(position);
        batch.sizes.push_back({1.0f, 1.0f, 1.0f});
    }

    for (const auto& pair : batches) {
        spine_batches.push_back(pair.second);
    }
}

//  ----------------------------------------------------------------------------
void DemoSystem::batch_sprites(
    Game& game,
    glm::mat4 view,
    glm::mat4 proj,
    std::vector<SpriteBatch>& sprite_batches
) {
    SystemManager& sys_mgr = game.get_system_manager();

    //  Get drawable entities
    const SpriteSystem& sprite_sys = get_sprite_system(sys_mgr);
    std::vector<Entity> entities;
    sprite_sys.get_entities(entities);

    const size_t entity_count = entities.size();

    std::map<uint32_t, SpriteBatch> batches;

    Frustum frustum(proj * view);

    const PositionSystem& pos_sys = get_position_system(sys_mgr);
    for (size_t n = 0; n < entity_count; ++n) {
        //  Get positions
        const auto pos_cmpnt = pos_sys.get_component(entities[n]);
        glm::vec3 position = pos_sys.get_position(pos_cmpnt);

        const auto sprite_cmpnt = sprite_sys.get_component(entities[n]);
        const uint32_t texture_id = sprite_sys.get_texture_id(sprite_cmpnt);
        const glm::vec2 size = sprite_sys.get_size(sprite_cmpnt);

        //  Sprite bounding box
        const glm::vec3 maxp(
            position.x + size.x,
            position.y + size.y,
            1.0f
        );

        const glm::vec3 minp(
            position.x - size.x,
            position.y - size.y,
            0.0f
        );

        //  Skip objects outside frustum
        if (!frustum.is_box_visible(minp, maxp)) {
            continue;
        }

        SpriteBatch& batch = batches[texture_id];
        batch.texture_id = texture_id;
        batch.positions.push_back(position);
        batch.sizes.push_back({size.x, size.y, 1.0f});
    }

    for (const auto& pair : batches) {
        sprite_batches.push_back(pair.second);
    }
}
}
