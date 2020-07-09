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

struct EntityInfo
{
    ecs::Entity entity;
    std::string name;
};

//  Builds a vector of debug infos for every entity
void build_entity_infos(
    Game& game,
    std::vector<EntityInfo>& entity_infos
);

//  Gets a filtered vector of debug info for entities in a system
void get_system_entity_info(
    const ecs::EntitySystemBase& system,
    const std::vector<EntityInfo>& entity_infos,
    std::vector<EntityInfo>& system_entity_infos
);
}
