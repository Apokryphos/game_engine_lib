#pragma once

#include "ecs/entity_manager.hpp"
#include "ecs/entity_system_base.hpp"
#include "common/system_id.hpp"
#include <cereal/types/vector.hpp>
#include <algorithm>
#include <memory>
#include <vector>

namespace ecs
{
class EcsRoot
{
    using SystemId = common::SystemId;

    EntityManager m_entity_manager;
    std::vector<Entity> m_entities;
    std::vector<Entity> m_removed_entities;
    std::vector<EntitySystemBase*> m_systems;

    void remove_expired_entities();

public:
    void add_system(EntitySystemBase* entity_system);
    Entity create_entity();
    void destroy_entity(const Entity entity);

    const std::vector<Entity>& get_entities() const {
        return m_entities;
    }

    template <typename T>
    T& get_system(const SystemId id) {
        auto find = std::find_if(
            m_systems.begin(),
            m_systems.end(),
            [id](const EntitySystemBase* system) {
                return system->get_id() == id;
            }
        );

        if (find == m_systems.end()) {
            throw std::runtime_error("System not found.");
        }

        EntitySystemBase* system = (*find);
        return *(static_cast<T*>(system));
    }

    bool is_alive(const Entity entity) const;

    template <typename Archive>
    void serialize(Archive& ar) {
        remove_expired_entities();

        ar(
            m_entities,
            m_removed_entities,
            // m_systems,
            m_entity_manager
        );
    }

    void update();
};
};
