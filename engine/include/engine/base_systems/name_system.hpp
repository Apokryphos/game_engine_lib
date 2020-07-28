#pragma once

#include "ecs/entity_system.hpp"
#include "engine/base_systems/base_system_ids.hpp"

namespace engine
{
struct NameComponentData
{
    std::string name;

    template <typename Archive>
    void archive(Archive& ar) {
        ar(name);
    }
};

class NameSystem : public ecs::EntitySystem<NameComponentData>
{
public:
    NameSystem(ecs::EcsRoot& ecs_root, unsigned int max_components)
    : EntitySystem(ecs_root, NameSystem::Id, "name_system", max_components) {
    }

    ecs::Entity get_entity(const std::string& name) const {
        const auto cmpnt_count = get_component_count();
        const auto& cmpnt_data = get_component_data();
        for (auto n = 0; n < cmpnt_count; ++n) {
            if (cmpnt_data[n].name == name) {
                return get_entity_by_component_index(n);
            }
        }

        throw std::runtime_error("Entity not found.");
    }

    const std::string& get_name(const Component cmpnt) const {
        return get_component_data(cmpnt).name;
    }

    static const common::SystemId Id = SYSTEM_ID_NAME;

    void set_name(const Component cmpnt, const std::string& name) {
        get_component_data(cmpnt).name = name;
    }
};
}
