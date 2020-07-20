#include "demo/systems/demo_system.hpp"
#include "engine/game.hpp"
#include "engine/system_manager.hpp"
#include "render/frustum.hpp"
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

        //  Skip models outside frustum
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
}
