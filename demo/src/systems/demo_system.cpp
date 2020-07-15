#include "demo/systems/demo_system.hpp"
#include "engine/game.hpp"
#include "engine/system_manager.hpp"
#include "systems/model_system.hpp"
#include "systems/position_system.hpp"
#include "systems/system_util.hpp"
#include <glm/glm.hpp>

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
    DrawOrder& draw_order
) {
    SystemManager& sys_mgr = game.get_system_manager();

    //  Get drawable entities
    const ModelSystem& model_sys = get_model_system(sys_mgr);
    std::vector<Entity> entities;
    model_sys.get_entities(entities);

    const size_t entity_count = entities.size();

    //  Get model IDs
    draw_order.model_ids.resize(entity_count);
    for (size_t n = 0; n < entity_count; ++n) {
        const auto model_cmpnt = model_sys.get_component(entities[n]);
        draw_order.model_ids[n] = model_sys.get_id(model_cmpnt);
    }

    //  Get positions
    draw_order.positions.resize(entity_count);
    const PositionSystem& pos_sys = get_position_system(sys_mgr);
    for (size_t n = 0; n < entity_count; ++n) {
        const auto pos_cmpnt = pos_sys.get_component(entities[n]);
        const glm::vec3 position = pos_sys.get_position(pos_cmpnt);
        draw_order.positions[n] = position;
    }

    //  Get texture IDs
    draw_order.texture_ids.resize(entity_count);
    for (size_t n = 0; n < entity_count; ++n) {
        draw_order.texture_ids[n] = 0;
    }
}
}
