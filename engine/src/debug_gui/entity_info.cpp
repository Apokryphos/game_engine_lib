#include "ecs/ecs_root.hpp"
#include "engine/base_systems/base_system_util.hpp"
#include "engine/debug_gui/entity_info.hpp"
#include "engine/game.hpp"
#include "engine/system_manager.hpp"
#include "engine/base_systems/name_system.hpp"

using namespace ecs;

namespace engine
{
//  ----------------------------------------------------------------------------
void get_system_entity_info(
    const EntitySystemBase& system,
    const std::vector<EntityInfo>& entity_infos,
    std::vector<EntityInfo>& system_entity_infos
) {
    system_entity_infos.clear();
    for (const EntityInfo info : entity_infos) {
        if (system.has_component(info.entity)) {
            system_entity_infos.push_back(info);
        }
    }
}

//  ----------------------------------------------------------------------------
void build_entity_infos(
    Game& game,
    std::vector<EntityInfo>& entity_infos
) {
    entity_infos.clear();

    const EcsRoot& ecs_root = game.get_ecs_root();
    const std::vector<Entity>& entities = ecs_root.get_entities();

    SystemManager& sys_mgr = game.get_system_manager();
    const NameSystem& name_sys = sys_mgr.get_system<NameSystem>(SYSTEM_ID_NAME);

    entity_infos.reserve(entities.size());
    for (const Entity entity : entities) {
        EntityInfo info{};
        info.entity = entity;

        if (name_sys.has_component(entity)) {
            const auto name_cmpnt = name_sys.get_component(entity);
            info.name = name_sys.get_name(name_cmpnt);
        } else {
            info.name = "entity_" + std::to_string(entity.id);
        }

        entity_infos.push_back(info);
    }
}
}
