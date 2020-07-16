#pragma once

//  Include base systems util for convenience since there
//  are few base systems.
#include "engine/base_systems/base_system_util.hpp"
#include <glm/vec3.hpp>

namespace engine
{
class SystemManager;
};

namespace systems
{
class CameraSystem;
class PositionSystem;
class ModelSystem;
class MoveSystem;

void add_camera_component(
    const ecs::Entity entity,
    CameraSystem& cam_sys,
    const glm::vec3 target
);

void add_model_component(
    const ecs::Entity entity,
    ModelSystem& model_sys,
    uint32_t model_id,
    uint32_t texture_id
);

void add_move_component(
    const ecs::Entity entity,
    MoveSystem& move_sys,
    float move_speed
);

void add_position_component(
    const ecs::Entity entity,
    PositionSystem& pos_sys,
    const glm::vec3 position
);

CameraSystem& get_camera_system(engine::SystemManager& sys_mgr);
ModelSystem& get_model_system(engine::SystemManager& sys_mgr);
MoveSystem& get_move_system(engine::SystemManager& sys_mgr);
PositionSystem& get_position_system(engine::SystemManager& sys_mgr);
}
