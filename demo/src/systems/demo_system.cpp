#include "demo/systems/demo_system.hpp"
#include "engine/game.hpp"
#include "engine/system_manager.hpp"
#include "systems/model_system.hpp"
#include "systems/position_system.hpp"
#include "systems/system_util.hpp"
#include <glm/glm.hpp>
#include <map>

using namespace ecs;
using namespace engine;
using namespace render;
using namespace systems;

namespace demo
{
//  ----------------------------------------------------------------------------
DemoSystem::DemoSystem()
: System(SYSTEM_ID_DEMO, "demo_system") {
}

//  ----------------------------------------------------------------------------
void DemoSystem::batch_models(
    Game& game,
    std::vector<ModelBatch>& model_batches
) {
    SystemManager& sys_mgr = game.get_system_manager();

    //  Get drawable entities
    const ModelSystem& model_sys = get_model_system(sys_mgr);
    std::vector<Entity> entities;
    model_sys.get_entities(entities);

    const size_t entity_count = entities.size();

    std::map<uint32_t, ModelBatch> batches;

    const PositionSystem& pos_sys = get_position_system(sys_mgr);
    for (size_t n = 0; n < entity_count; ++n) {
        const auto model_cmpnt = model_sys.get_component(entities[n]);
        const uint32_t model_id = model_sys.get_model_id(model_cmpnt);
        ModelBatch& batch = batches[model_id];

        batch.model_id = model_id;

        //  Get positions
        const auto pos_cmpnt = pos_sys.get_component(entities[n]);
        const glm::vec3 position = pos_sys.get_position(pos_cmpnt);
        batch.positions.push_back(position);
    }

    for (size_t n = 0; n < entity_count; ++n) {
        const auto model_cmpnt = model_sys.get_component(entities[n]);
        const uint32_t model_id = model_sys.get_model_id(model_cmpnt);
        ModelBatch& batch = batches[model_id];

        //  Get texture IDs
        batch.texture_ids.push_back(model_sys.get_texture_id(model_cmpnt));
    }

    for (const auto& pair : batches) {
        model_batches.push_back(pair.second);
    }
}
}
