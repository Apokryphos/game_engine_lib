#include "engine/system_manager.hpp"
#include "systems/camera_system.hpp"
#include "systems/model_system.hpp"
#include "systems/move_system.hpp"
#include "systems/position_system.hpp"
#include "systems/sprite_system.hpp"
#include "systems/system_util.hpp"
#include <glm/vec3.hpp>

using namespace ecs;
using namespace engine;

namespace systems
{
//  ----------------------------------------------------------------------------
void activate_camera(const ecs::Entity entity, CameraSystem& cam_sys) {
    const auto cam_cmpnt = cam_sys.get_component(entity);
    cam_sys.activate(cam_cmpnt);
}

//  ----------------------------------------------------------------------------
void add_camera_component(
    const Entity entity,
    CameraSystem& cam_sys,
    const glm::vec3 target
) {
    if (!cam_sys.has_component(entity)) {
        cam_sys.add_component(entity);
    }

    const auto cam_cmpnt = cam_sys.get_component(entity);
    cam_sys.set_target(cam_cmpnt, target);
}

//  ----------------------------------------------------------------------------
void add_model_component(
    const Entity entity,
    ModelSystem& model_sys,
    uint32_t model_id,
    uint32_t texture_id
) {
    if (!model_sys.has_component(entity)) {
        model_sys.add_component(entity);
    }

    const auto model_cmpnt = model_sys.get_component(entity);
    model_sys.set_model_id(model_cmpnt, model_id);
    model_sys.set_texture_id(model_cmpnt, texture_id);
}

//  ----------------------------------------------------------------------------
void add_move_component(
    const Entity entity,
    MoveSystem& move_sys,
    float move_speed
) {
    if (!move_sys.has_component(entity)) {
        move_sys.add_component(entity);
    }

    const auto move_cmpnt = move_sys.get_component(entity);
    move_sys.set_move_speed(move_cmpnt, move_speed);
}

//  ----------------------------------------------------------------------------
void add_position_component(
    const Entity entity,
    PositionSystem& pos_sys,
    const glm::vec3 position
) {
    if (!pos_sys.has_component(entity)) {
        pos_sys.add_component(entity);
    }

    const auto pos_cmpnt = pos_sys.get_component(entity);
    pos_sys.set_position(pos_cmpnt, position);
}

//  ----------------------------------------------------------------------------
void add_sprite_component(
    const Entity entity,
    SpriteSystem& sprite_sys,
    uint32_t texture_id,
    const glm::vec2 size
) {
    if (!sprite_sys.has_component(entity)) {
        sprite_sys.add_component(entity);
    }

    const auto sprite_cmpnt = sprite_sys.get_component(entity);
    sprite_sys.set_texture_id(sprite_cmpnt, texture_id);
    sprite_sys.set_size(sprite_cmpnt, size);
}

//  ----------------------------------------------------------------------------
CameraSystem& get_camera_system(SystemManager& sys_mgr) {
    return sys_mgr.get_system<CameraSystem>(SYSTEM_ID_CAMERA);
}

//  ----------------------------------------------------------------------------
ModelSystem& get_model_system(SystemManager& sys_mgr) {
    return sys_mgr.get_system<ModelSystem>(SYSTEM_ID_MODEL);
}

//  ----------------------------------------------------------------------------
MoveSystem& get_move_system(SystemManager& sys_mgr) {
    return sys_mgr.get_system<MoveSystem>(SYSTEM_ID_MOVE);
}

//  ----------------------------------------------------------------------------
PositionSystem& get_position_system(SystemManager& sys_mgr) {
    return sys_mgr.get_system<PositionSystem>(SYSTEM_ID_POSITION);
}

//  ----------------------------------------------------------------------------
SpriteSystem& get_sprite_system(engine::SystemManager& sys_mgr) {
    return sys_mgr.get_system<SpriteSystem>(SYSTEM_ID_SPRITE);
}
}
