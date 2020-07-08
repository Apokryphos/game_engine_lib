#pragma once

#include "ecs/entity_system.hpp"
#include "systems/system_ids.hpp"
#include <glm/vec3.hpp>

namespace systems
{
struct PositionComponentData
{
    glm::vec3 position;

    template <typename Archive>
    void archive(Archive& ar) {
        ar(position);
    }
};

class PositionSystem : public ecs::EntitySystem<PositionComponentData>
{
public:
    PositionSystem(ecs::EcsRoot& ecs_root, unsigned int max_components)
    : EntitySystem(ecs_root, PositionSystem::Id, "position_system", max_components) {
    }

    const glm::vec3& get_position(const Component cmpnt) const {
        return get_component_data(cmpnt).position;
    }

    static const common::SystemId Id = SYSTEM_ID_POSITION;

    void set_position(const Component cmpnt, const glm::vec3& position) {
        get_component_data(cmpnt).position = position;
    }
};
}
