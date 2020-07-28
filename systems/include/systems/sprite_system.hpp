#pragma once

#include "ecs/entity_system.hpp"
#include "systems/system_ids.hpp"

namespace systems
{
struct SpriteComponentData
{
    uint32_t texture_id;

    template <typename Archive>
    void archive(Archive& ar) {
        ar(
            texture_id
        );
    }
};

class SpriteSystem : public ecs::EntitySystem<SpriteComponentData>
{
public:
    SpriteSystem(ecs::EcsRoot& ecs_root, unsigned int max_components)
    : EntitySystem(ecs_root, SpriteSystem::Id, "sprite_system", max_components) {
    }

    uint32_t get_texture_id(const Component cmpnt) const {
        return get_component_data(cmpnt).texture_id;
    }

    static const common::SystemId Id = SYSTEM_ID_SPRITE;

    void set_texture_id(const Component cmpnt, const uint32_t texture_id) {
        get_component_data(cmpnt).texture_id = texture_id;
    }
};
}
