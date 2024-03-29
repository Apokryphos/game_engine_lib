#pragma once

#include "ecs/entity_system.hpp"
#include "systems/system_ids.hpp"

namespace systems
{
struct ModelComponentData
{
    uint32_t model_id;
    uint32_t texture_id;

    template <typename Archive>
    void archive(Archive& ar) {
        ar(
            model_id,
            texture_id
        );
    }
};

class ModelSystem : public ecs::EntitySystem<ModelComponentData>
{
public:
    ModelSystem(ecs::EcsRoot& ecs_root, unsigned int max_components)
    : EntitySystem(ecs_root, ModelSystem::Id, "model_system", max_components) {
    }

    uint32_t get_model_id(const Component cmpnt) const {
        return get_component_data(cmpnt).model_id;
    }

    uint32_t get_texture_id(const Component cmpnt) const {
        return get_component_data(cmpnt).texture_id;
    }

    static const common::SystemId Id = SYSTEM_ID_MODEL;

    void set_model_id(const Component cmpnt, const uint32_t model_id) {
        get_component_data(cmpnt).model_id = model_id;
    }

    void set_texture_id(const Component cmpnt, const uint32_t texture_id) {
        get_component_data(cmpnt).texture_id = texture_id;
    }
};
}
