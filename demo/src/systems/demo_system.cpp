#include "demo/systems/demo_system.hpp"
#include "engine/game.hpp"
#include "engine/system_manager.hpp"
#include "systems/model_system.hpp"
#include "systems/position_system.hpp"
#include "systems/system_util.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace ecs;
using namespace engine;
using namespace systems;

namespace demo
{
//  ----------------------------------------------------------------------------
DemoSystem::DemoSystem()
: System(SYSTEM_ID_DEMO, "demo_system") {
}

//  ----------------------------------------------------------------------------
void DemoSystem::build_draw_order(
    Game& game,
    std::vector<DrawOrder>& draw_order
) {
    SystemManager& sys_mgr = game.get_system_manager();

    //  Get drawable entities
    const ModelSystem& model_sys = get_model_system(sys_mgr);
    std::vector<Entity> entities;
    model_sys.get_entities(entities);

    //  Get model IDs
    const size_t entity_count = entities.size();
    draw_order.resize(entities.size());
    for (size_t n = 0; n < entity_count; ++n) {
        const auto model_cmpnt = model_sys.get_component(entities[n]);
        draw_order[n].model_id = model_sys.get_id(model_cmpnt);
    }

    //  Get positions
    const PositionSystem& pos_sys = get_position_system(sys_mgr);
    for (size_t n = 0; n < entity_count; ++n) {
        const auto pos_cmpnt = pos_sys.get_component(entities[n]);
        const glm::vec3 position = pos_sys.get_position(pos_cmpnt);
        draw_order[n].model = glm::translate(glm::mat4(1.0f), position);
    }
}
}
