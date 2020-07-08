#include "ecs/entity_system.hpp"
#include "ecs/ecs_root.hpp"

namespace ecs
{
//  ----------------------------------------------------------------------------
void EcsRoot::add_system(EntitySystemBase* entity_system) {
    m_systems.push_back(entity_system);
}

//  ----------------------------------------------------------------------------
Entity EcsRoot::create_entity() {
    Entity entity = m_entity_manager.create_entity();
    m_entities.push_back(entity);
    return entity;
}

//  ----------------------------------------------------------------------------
void EcsRoot::destroy_entity(const Entity entity) {
    m_entity_manager.destroy_entity(entity);
    m_removed_entities.push_back(entity);
}

//  ----------------------------------------------------------------------------
bool EcsRoot::is_alive(const Entity entity) const {
    return m_entity_manager.is_alive(entity);
}

//  ----------------------------------------------------------------------------
void EcsRoot::remove_expired_entities() {
    if (m_removed_entities.size() == 0) {
        return;
    }

    std::sort(m_entities.begin(), m_entities.end());
    std::sort(m_removed_entities.begin(), m_removed_entities.end());

    //  Removed expired entities from list of entities
    m_entities.erase(
        std::remove_if( begin(m_entities),end(m_entities),
        [&](auto x) {
            return find(
                std::begin(m_removed_entities),
                std::end(m_removed_entities),
                x) != std::end(m_removed_entities);
        }),
        end(m_entities));

    //  Remove expired entities from all systems
    for (const Entity& entity : m_removed_entities) {
        for (EntitySystemBase* system : m_systems) {
            if (system->has_component(entity)) {
                system->remove_component(entity);
            }
        }
    }

    m_removed_entities.clear();
}

//  ----------------------------------------------------------------------------
void EcsRoot::update() {
    remove_expired_entities();
}
}
