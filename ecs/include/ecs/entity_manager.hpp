#pragma once

#include "ecs/entity.hpp"
#include <cereal/types/deque.hpp>
#include <cereal/types/vector.hpp>
#include <deque>
#include <istream>
#include <ostream>
#include <vector>

namespace ecs
{
class EntityManager
{
    const unsigned MINIMUM_FREE_INDICES = 1024;

    std::deque<unsigned> m_free_indices;
    std::vector<unsigned> m_generations;

public:
    EntityManager();
    EntityManager(const EntityManager&) = delete;
    EntityManager& operator=(const EntityManager&) = delete;
    Entity create_entity();
    void destroy_entity(const Entity entity);
    bool is_alive(const Entity entity) const;

    template <typename Archive>
    void serialize(Archive& ar) {
        ar(
            m_free_indices,
            m_generations
        );
    }
};
}
