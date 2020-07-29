#pragma once

#include "ecs/entity_system.hpp"
#include "systems/system_ids.hpp"
#include <glm/vec2.hpp>

namespace systems
{
struct BillboardComponentData
{
    uint32_t texture_id;
    glm::vec2 size;

    template <typename Archive>
    void archive(Archive& ar) {
        ar(
            texture_id,
            size
        );
    }
};

class BillboardSystem : public ecs::EntitySystem<BillboardComponentData>
{
public:
    BillboardSystem(ecs::EcsRoot& ecs_root, unsigned int max_components)
    : EntitySystem(ecs_root, BillboardSystem::Id, "billboard_system", max_components) {
    }

    glm::vec2 get_size(const Component cmpnt) const {
        return get_component_data(cmpnt).size;
    }

    uint32_t get_texture_id(const Component cmpnt) const {
        return get_component_data(cmpnt).texture_id;
    }

    static const common::SystemId Id = SYSTEM_ID_BILLBOARD;

    void set_size(const Component cmpnt, const glm::vec2 size)  {
        get_component_data(cmpnt).size = size;
    }

    void set_texture_id(const Component cmpnt, const uint32_t texture_id) {
        get_component_data(cmpnt).texture_id = texture_id;
    }
};
}
