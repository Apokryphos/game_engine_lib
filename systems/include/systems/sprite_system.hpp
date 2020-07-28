#pragma once

#include "ecs/entity_system.hpp"
#include "systems/system_ids.hpp"

namespace systems
{
struct SpriteComponentData
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

class SpriteSystem : public ecs::EntitySystem<SpriteComponentData>
{
public:
    SpriteSystem(ecs::EcsRoot& ecs_root, unsigned int max_components)
    : EntitySystem(ecs_root, SpriteSystem::Id, "sprite_system", max_components) {
    }

    glm::vec2 get_size(const Component cmpnt) const {
        return get_component_data(cmpnt).size;
    }

    uint32_t get_texture_id(const Component cmpnt) const {
        return get_component_data(cmpnt).texture_id;
    }

    static const common::SystemId Id = SYSTEM_ID_SPRITE;

    void set_size(const Component cmpnt, const glm::vec2 size)  {
        get_component_data(cmpnt).size = size;
    }

    void set_texture_id(const Component cmpnt, const uint32_t texture_id) {
        get_component_data(cmpnt).texture_id = texture_id;
    }
};
}
