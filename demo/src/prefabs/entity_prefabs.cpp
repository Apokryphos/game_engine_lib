#include "ecs/entity.hpp"
#include "engine/game.hpp"
#include "systems/camera_system.hpp"
#include "systems/system_util.hpp"
#include <glm/vec3.hpp>

using namespace ecs;
using namespace engine;
using namespace systems;

namespace demo
{
//  ----------------------------------------------------------------------------
void make_camera(
    Game& game,
    const Entity entity,
    const std::string& name,
    glm::vec3 position,
    float move_speed,
    bool ortho
) {
    SystemManager& sys_mgr = game.get_system_manager();
    add_name_component(entity, get_name_system(sys_mgr), name);
    add_move_component(entity, get_move_system(sys_mgr), move_speed);
    add_position_component(entity, get_position_system(sys_mgr), position);

    //  Camera component
    CameraSystem& cam_sys = get_camera_system(sys_mgr);
    add_camera_component(
        entity,
        cam_sys,
        glm::vec3(0)
    );
    const auto cam_cmpnt = cam_sys.get_component(entity);
    cam_sys.set_ortho(cam_cmpnt, ortho);

    if (!ortho) {
        cam_sys.set_mode(cam_cmpnt, CameraMode::Orbit);
    }
}
}
