#pragma once

#include "ecs/entity.hpp"
#include <cassert>

namespace ecs
{
class EntityHandle
{
    bool m_assigned = false;
    Entity m_entity;

public:
    EntityHandle()
    : m_entity(0) {
    }

    bool equals(const EntityHandle& handle) const {
        if (!m_assigned) {
            return !handle.m_assigned;
        }

        return m_entity == handle.m_entity;
    }

    Entity get_entity() const {
        assert(m_assigned);
        return m_entity;
    }

    bool has_entity() const {
        return m_assigned;
    }

    bool is_entity(const Entity entity) const {
        return m_assigned && m_entity == entity;
    }

    void reset() {
        m_assigned = false;
        m_entity = Entity(0);
    }

    template <typename Archive>
    void serialize(Archive& ar) {
        ar(
            m_assigned,
            m_entity
        );
    }

    void set_entity(Entity entity) {
        m_assigned = true;
        m_entity = entity;
    }
};
}
