#include "ecs/ecs_root.hpp"
#include "engine/base_systems/base_system_util.hpp"
#include "engine/debug_gui/entity_debug_info.hpp"
#include "engine/game.hpp"
#include "engine/system_manager.hpp"
#include "engine/base_systems/name_system.hpp"

using namespace ecs;

namespace engine
{
//  ----------------------------------------------------------------------------
void get_system_entity_debug_info(
    const EntitySystemBase& system,
    const std::vector<EntityDebugInfo>& entity_debug_infos,
    std::vector<EntityDebugInfo>& system_entity_debug_infos
) {
    for (const EntityDebugInfo debug_info : entity_debug_infos) {
        if (system.has_component(debug_info.entity)) {
            system_entity_debug_infos.push_back(debug_info);
        }
    }
}

//  ----------------------------------------------------------------------------
void build_entity_debug_infos(
    Game& game,
    std::vector<EntityDebugInfo>& entity_debug_infos
) {
    const EcsRoot& ecs_root = game.get_ecs_root();
    const std::vector<Entity>& entities = ecs_root.get_entities();

    SystemManager& sys_mgr = game.get_system_manager();
    const NameSystem& name_sys = sys_mgr.get_system<NameSystem>(SYSTEM_ID_NAME);

    entity_debug_infos.reserve(entities.size());
    for (const Entity entity : entities) {
        EntityDebugInfo info{};
        info.entity = entity;

        if (name_sys.has_component(entity)) {
            const auto name_cmpnt = name_sys.get_component(entity);
            info.name = name_sys.get_name(name_cmpnt);
        } else {
            info.name = "entity_" + std::to_string(entity.id);
        }

        entity_debug_infos.push_back(info);
    }
}
}
