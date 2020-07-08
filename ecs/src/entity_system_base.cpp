#include "ecs/ecs_root.hpp"
#include "ecs/entity_system_base.hpp"
#include <cassert>

namespace ecs
{
//  ----------------------------------------------------------------------------
EntitySystemBase::EntitySystemBase(
    EcsRoot& ecs_root,
    const SystemId id,
    const std::string& name,
    unsigned int max_components
) : System(id, name),
    m_component_count(0),
    m_ecs_root(ecs_root),
    max_components(max_components)
{
    m_component_indices_by_entity.reserve(max_components);
    m_entities_by_component_indices.reserve(max_components);
}

//  ----------------------------------------------------------------------------
void EntitySystemBase::add_component(const Entity entity) {
    assert(m_component_indices_by_entity.size() < this->max_components);

    int cmpntIndex = static_cast<int>(m_component_indices_by_entity.size());

    m_component_indices_by_entity.emplace(entity, cmpntIndex);
    m_entities_by_component_indices.emplace(cmpntIndex, entity);

    ++m_component_count;

    this->create_component();
}

//  ----------------------------------------------------------------------------
bool EntitySystemBase::entity_is_alive(const Entity entity) const {
    return m_ecs_root.is_alive(entity);
}

//  ----------------------------------------------------------------------------
void EntitySystemBase::garbage_collect() {
    if (m_component_indices_by_entity.size() > 0) {
        unsigned aliveInRow = 0;

        while (m_component_indices_by_entity.size() > 0 && aliveInRow < 4) {
            //  Random index
            unsigned e = rand() % m_component_indices_by_entity.size();

            auto pair = std::next(std::begin(m_component_indices_by_entity), e);

            if (m_ecs_root.is_alive(pair->first)) {
                ++aliveInRow;
                continue;
            }

            aliveInRow = 0;

            this->remove_component(pair->first);
        }
    }
}

//  ----------------------------------------------------------------------------
size_t EntitySystemBase::get_component_count() const {
    return m_component_count;
}

//  ----------------------------------------------------------------------------
ComponentIndex EntitySystemBase::get_component_index_by_entity(
    const Entity entity
) const {
    return m_component_indices_by_entity.at(entity);
}

//  ----------------------------------------------------------------------------
std::vector<Entity> EntitySystemBase::get_entities() const {
    std::vector<Entity> entities;
    entities.reserve(m_component_indices_by_entity.size());

    for(auto& p : m_component_indices_by_entity) {
        entities.push_back(p.first);
    }

    return entities;
}

//  ----------------------------------------------------------------------------
void EntitySystemBase::get_entities(std::vector<Entity>& entities) const {
    for(auto& p : m_component_indices_by_entity) {
        entities.push_back(p.first);
    }
}

//  ----------------------------------------------------------------------------
const Entity EntitySystemBase::get_entity_by_component_index(
    const ComponentIndex index
) const {
    return m_entities_by_component_indices.at(index);
}

//  ----------------------------------------------------------------------------
EcsRoot& EntitySystemBase::get_ecs_root() {
    return m_ecs_root;
}

//  ----------------------------------------------------------------------------
const EcsRoot& EntitySystemBase::get_ecs_root() const {
    return m_ecs_root;
}

//  ----------------------------------------------------------------------------
bool EntitySystemBase::has_component(const Entity entity) const {
    return m_component_indices_by_entity.find(entity) !=
           m_component_indices_by_entity.end();
}

//  ----------------------------------------------------------------------------
void EntitySystemBase::remove_component(const Entity entity) {
    int last       = static_cast<int>(m_component_indices_by_entity.size() - 1);
    int cmpntIndex = m_component_indices_by_entity.at(entity);

    --m_component_count;

    //  In each component list, swap the specified entity's component
    //  with the last component in the list
    this->destroy_component(cmpntIndex);

    //  Update map to point to swapped component index
    //  Use at to ensure entry actually exists
    const Entity lastEntity = m_entities_by_component_indices.at(last);
    m_component_indices_by_entity[lastEntity] = cmpntIndex;
    //  Erase after updating swapped component index in case the
    //  component being removed is the only one in the system.
    //  unordered_map.erase() doesn't fail if the specified key doesn't
    //  exist, so there's no need to check the size of the map before
    //  attempting to erase.
    m_component_indices_by_entity.erase(entity);

    //  Have to check if the component being removed is the last one
    //  or else the map emplace call would add an entry for the removed
    //  component.
    m_entities_by_component_indices.erase(cmpntIndex);
    if (entity != lastEntity) {
        //  Erase the entry for the last entity in map
        m_entities_by_component_indices.erase(last);
        //  Add new entry for the former last entity and swapped component index
        m_entities_by_component_indices.emplace(cmpntIndex, lastEntity);
    }
}
}
