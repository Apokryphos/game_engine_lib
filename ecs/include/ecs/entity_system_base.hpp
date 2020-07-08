#pragma once

#include "common/system.hpp"
#include "ecs/entity.hpp"
#include <cereal/types/base_class.hpp>
#include <cereal/types/unordered_map.hpp>
#include <cereal/types/vector.hpp>
#include <unordered_map>
#include <vector>

namespace ecs
{
class EcsRoot;

using ComponentIndex = EntityId;

class EntitySystemBase : public common::System
{
    using SystemId = common::SystemId;

    size_t m_component_count;
    EcsRoot& m_ecs_root;
    std::unordered_map<Entity, ComponentIndex> m_component_indices_by_entity;
    std::unordered_map<ComponentIndex, Entity> m_entities_by_component_indices;

protected:
    virtual void create_component() = 0;
    virtual void destroy_component(ComponentIndex index) = 0;
    bool entity_is_alive(const Entity entity) const;
    ComponentIndex get_component_index_by_entity(const Entity entity) const;
    const Entity get_entity_by_component_index(const ComponentIndex index) const;
    EcsRoot& get_ecs_root();
    const EcsRoot& get_ecs_root() const;

public:
    EntitySystemBase(
        EcsRoot& ecs_root,
        const SystemId id,
        const std::string& name,
        unsigned int max_components
    );
    EntitySystemBase(const EntitySystemBase&) = delete;
    EntitySystemBase& operator=(const EntitySystemBase&) = delete;
    ~EntitySystemBase() {}
    void add_component(const Entity entity);
    void garbage_collect();
    size_t get_component_count() const;
    std::vector<Entity> get_entities() const;
    void get_entities(std::vector<Entity>& entities) const;
    bool has_component(const Entity entity) const;
    const size_t max_components;
    void remove_component(const Entity entity);

    template <typename Archive>
    void serialize(Archive& ar) {
        ar(
            cereal::base_class<EntitySystemBase>(this),
            m_id,
            m_component_count,
            m_component_indices_by_entity,
            m_entities_by_component_indices
        );
    }
};
}
