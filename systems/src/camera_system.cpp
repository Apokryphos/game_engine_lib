#include "ecs/ecs_root.hpp"
#include "systems/camera_system.hpp"
#include "systems/move_system.hpp"
#include "systems/position_system.hpp"
#include "systems/system_util.hpp"
#include "engine/game.hpp"
#include "engine/time.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

using namespace ecs;
using namespace engine;

namespace systems
{
//  ----------------------------------------------------------------------------
void CameraSystem::initialize_component_data(size_t index, CameraComponentData& data) {
    data.ortho = false;
    data.zoom_speed = 10.0f;
}

//  ----------------------------------------------------------------------------
void CameraSystem::update(Game& game) {
    const float elapsed_seconds = get_elapsed_seconds();

    SystemManager& sys_mgr = game.get_system_manager();
    MoveSystem& move_sys = get_move_system(sys_mgr);
    PositionSystem& pos_sys = get_position_system(sys_mgr);

    const auto cmpnt_count = get_component_count();
    for (auto cmpnt_index = 0; cmpnt_index < cmpnt_count; ++cmpnt_index) {
        const Entity camera = get_entity_by_component_index(cmpnt_index);

        //  Get camera direction (rotation)
        const auto camera_move_cmpnt = move_sys.get_component(camera);
        glm::vec3 facing = move_sys.get_facing(camera_move_cmpnt);

        //  Get camera position
        const auto camera_pos_cmpnt = pos_sys.get_component(camera);
        glm::vec3 camera_pos = pos_sys.get_position(camera_pos_cmpnt);

        auto& data = get_component_data(cmpnt_index);

        //  Update zoom
        data.distance = std::clamp(
            data.distance + data.zoom_direction * data.zoom_speed * elapsed_seconds,
            0.1f,
            10.0f
        );

        //  Get target position (entity or vec3)
        glm::vec3 target_pos;
        if (data.target_entity.has_entity()) {
            const Entity target = data.target_entity.get_entity();
            const auto target_pos_cmpnt = pos_sys.get_component(target);
            glm::vec3 target_pos = pos_sys.get_position(target_pos_cmpnt);
        } else {
            target_pos = data.target;
        }

        //  Calculate camera up vector
        glm::vec3 camera_up{0.0f, 0.0f, 1.0f};

        //  Ortho camera
        if (data.ortho) {
            camera_pos = {
                camera_pos.x,
                camera_pos.y,
                0.0f
            };
            target_pos = {
                camera_pos.x,
                camera_pos.y,
                -1.0f
            };
            camera_up = glm::vec3(0.0f, 1.0f, 0.0f);
        }

        //  Camera mode logic
        if (data.mode == CameraMode::Aim) {
            //  Position and target are used to calculate view matrix.
            //  Neither is modified.
            data.view = glm::lookAt(camera_pos, target_pos, camera_up);
        } else if (data.mode == CameraMode::Orbit) {
            glm::vec3 eye = camera_pos - facing * data.distance;
            eye.z += data.distance;

            data.view = glm::lookAt(eye, camera_pos, camera_up);
        } else {
            throw std::runtime_error("Not implemented.");
        }

        data.zoom_direction = 0;
    }
}
}
