#include "ecs/entity_manager.hpp"

namespace ecs
{
//  ----------------------------------------------------------------------------
EntityManager::EntityManager() {
    m_generations.reserve(MINIMUM_FREE_INDICES);
}

//  ----------------------------------------------------------------------------
Entity EntityManager::create_entity() {
    unsigned index;

    if (m_free_indices.size() > MINIMUM_FREE_INDICES) {
        index = m_free_indices.front();
        m_free_indices.pop_front();
    } else {
        m_generations.push_back(0);
        index = (int)m_generations.size() - 1;
    }

    return Entity(index, m_generations[index]);
}

//  ----------------------------------------------------------------------------
void EntityManager::destroy_entity(const Entity entity) {
    const unsigned index = entity.index();
    ++m_generations[index];
    m_free_indices.push_back(index);
}

//  ----------------------------------------------------------------------------
bool EntityManager::is_alive(const Entity entity) const {
    return m_generations[entity.index()] == entity.generation();
}
}
