#pragma once

#include "ecs/entity.hpp"
#include <string>

namespace ecs
{
class EntitySystemBase;
}

namespace engine
{
class Game;

struct EntityDebugInfo
{
    ecs::Entity entity;
    std::string name;
};

//  Builds a vector of debug infos for every entity
void build_entity_debug_infos(
    Game& game,
    std::vector<EntityDebugInfo>& entity_debug_infos
);

//  Gets a filtered vector of debug info for entities in a system
void get_system_entity_debug_info(
    const ecs::EntitySystemBase& system,
    const std::vector<EntityDebugInfo>& entity_debug_infos,
    std::vector<EntityDebugInfo>& system_entity_debug_infos
);
}
