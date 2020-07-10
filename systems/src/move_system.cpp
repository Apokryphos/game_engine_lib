#include "engine/game.hpp"
#include "engine/time.hpp"
#include "systems/move_system.hpp"
#include "systems/position_system.hpp"
#include "systems/system_util.hpp"
#include <glm/gtx/rotate_vector.hpp>

using namespace ecs;
using namespace engine;

namespace systems
{
//  ----------------------------------------------------------------------------
glm::vec3 MoveSystem::get_facing(const Component cmpnt) const {
    auto& data = get_component_data(cmpnt.index);
    return glm::rotateZ(glm::vec3(0.0f, 1.0f, 0.0f), data.direction);
}

//  ----------------------------------------------------------------------------
void MoveSystem::initialize_component_data(
    size_t index,
    MoveComponentData& data
) {
    data.move_speed = 5.0f;
    data.turn_speed = glm::radians(180.0f);
}

//  ----------------------------------------------------------------------------
void MoveSystem::update(engine::Game& game) {
    const float elapsed_seconds = get_elapsed_seconds();

    SystemManager& sys_mgr = game.get_system_manager();
    PositionSystem& pos_sys = get_position_system(sys_mgr);

    const auto cmpnt_count = get_component_count();
    for (auto cmpnt_index = 0; cmpnt_index < cmpnt_count; ++cmpnt_index) {
        const Entity entity = get_entity_by_component_index(cmpnt_index);

        auto& data = get_component_data(cmpnt_index);

        //  Update direction
        data.direction += data.turn * data.turn_speed * elapsed_seconds;

        //  Get position
        const auto pos_cmpnt = pos_sys.get_component(entity);
        glm::vec3 position = pos_sys.get_position(pos_cmpnt);

        //  Update position
        glm::vec3 delta = glm::rotateZ(data.move, data.direction) * data.turn_speed * elapsed_seconds;
        pos_sys.set_position(pos_cmpnt, position + delta);

        //  Reset for next frame
        data.move = glm::vec3(0);
        data.turn = 0;
    }
}
}
