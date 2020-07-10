#include "ecs/ecs_root.hpp"
#include "systems/camera_system.hpp"
#include "systems/position_system.hpp"
#include "systems/system_util.hpp"
#include "engine/game.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace ecs;
using namespace engine;

namespace systems
{
//  ----------------------------------------------------------------------------
void CameraSystem::update(Game& game) {
    SystemManager& sys_mgr = game.get_system_manager();
    PositionSystem& pos_sys = get_position_system(sys_mgr);

    const auto cmpnt_count = get_component_count();
    for (auto cmpnt_index = 0; cmpnt_index < cmpnt_count; ++cmpnt_index) {
        const Entity camera = get_entity_by_component_index(cmpnt_index);

        //  Get camera position
        const auto camera_pos_cmpnt = pos_sys.get_component(camera);
        glm::vec3 camera_pos = pos_sys.get_position(camera_pos_cmpnt);

        auto& data = get_component_data(cmpnt_index);

        //  Get target position (entity or vec3)
        glm::vec3 target_pos;
        if (data.target_entity.has_entity()) {
            const Entity target = data.target_entity.get_entity();
            const auto target_pos_cmpnt = pos_sys.get_component(target);
            glm::vec3 target_pos = pos_sys.get_position(target_pos_cmpnt);
        } else {
            target_pos = data.target;
        }

        //  Camera mode logic
        if (data.mode == CameraMode::Aim) {
            //  Position and target are used to calculate view matrix.
            //  Neither is modified.
            data.view = glm::lookAt(
                camera_pos,
                target_pos,
                glm::vec3(0.0f, 0.0f, 1.0f)
            );
        } else if (data.mode == CameraMode::Orbit) {
            throw std::runtime_error("Not implemented.");
        } else {
            throw std::runtime_error("Not implemented.");
        }
    }
}
}
