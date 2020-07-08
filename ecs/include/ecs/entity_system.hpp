#pragma once

#include "common/vector.hpp"
#include "ecs/entity_system_base.hpp"
#include <cereal/types/vector.hpp>

namespace ecs
{
template <typename T>
class EntitySystem : public EntitySystemBase
{
    using SystemId = common::SystemId;

public:
    struct Component
    {
        Component(const ComponentIndex index) {
            this->index = index;
        }

        ComponentIndex index;
    };

private:
    //  Component data
    std::vector<T> m_data;

protected:
    virtual void initialize_component_data(size_t index, T& data) {};
    virtual void release_component_data(size_t index, T& data) {};

    virtual void create_component() final {
        assert(m_data.capacity() > m_data.size());

        m_data.emplace_back();

        const size_t index = m_data.size() - 1;
        T& data = m_data.back();

        initialize_component_data(index, data);
    }

    virtual void destroy_component(ComponentIndex index) override {
        release_component_data(index, m_data.at(index));
        common::swap_remove(m_data, index);
    };

    T& get_component_data(const Component cmpnt) {
        return m_data.at(cmpnt.index);
    }

    T& get_component_data(const ComponentIndex& index) {
        return m_data.at(index);
    }

    const T& get_component_data(const Component cmpnt) const {
        return m_data.at(cmpnt.index);
    }

    const T& get_component_data(const ComponentIndex& index) const {
        return m_data.at(index);
    }

    std::vector<T>& get_component_data() {
        return m_data;
    }

    const std::vector<T>& get_component_data() const {
        return m_data;
    }

public:
    EntitySystem(
        EcsRoot& ecs_root,
        const SystemId id,
        const std::string& name,
        unsigned int max_components
    )
    : EntitySystemBase(ecs_root, id, name, max_components) {
        m_data.reserve(max_components);
    }

    ~EntitySystem() {}

    Component get_component(const Entity entity) const {
        return Component(get_component_index_by_entity(entity));
    }

    //  Gets a copy of the component data
    //  Intended for initialization, editor, debugging, etc.
    void get_component_data(const Component cmpnt, T& data) const {
        data = m_data.at(cmpnt.index);
    }

    template <typename Archive>
    void serialize(Archive& ar) {
        //  Components
        ar(
            cereal::base_class<EntitySystemBase>(this),
            m_data
        );
    }

    //  Sets component data
    //  Intended for initialization, editor, debugging, etc.
    void set_component_data(const Component cmpnt, const T& data) {
        m_data.at(cmpnt.index) = data;
    }
};
}
