#pragma once

#include "assets/asset_id.hpp"
#include "ecs/entity_system.hpp"
#include "systems/system_ids.hpp"

namespace systems
{
struct SpineComponentData
{
    assets::AssetId spine_id;

    template <typename Archive>
    void archive(Archive& ar) {
        ar(
            spine_id
        );
    }
};

class SpineSystem : public ecs::EntitySystem<SpineComponentData>
{
public:
    SpineSystem(ecs::EcsRoot& ecs_root, unsigned int max_components)
    : EntitySystem(ecs_root, SpineSystem::Id, "spine_system", max_components) {
    }

    assets::AssetId get_spine_id(const Component cmpnt) const {
        return get_component_data(cmpnt).spine_id;
    }

    static const common::SystemId Id = SYSTEM_ID_SPINE;

    void set_spine_id(const Component cmpnt, const assets::AssetId spine_id) {
        get_component_data(cmpnt).spine_id = spine_id;
    }
};
}
