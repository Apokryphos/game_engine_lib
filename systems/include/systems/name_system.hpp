#pragma once

#include "ecs/entity_system.hpp"
#include "systems/system_ids.hpp"

namespace systems
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

    const std::string& get_name(const Component cmpnt) const {
        return get_component_data(cmpnt).name;
    }

    static const common::SystemId Id = SYSTEM_ID_NAME;

    void set_name(const Component cmpnt, const std::string& name) {
        get_component_data(cmpnt).name = name;
    }
};
}
