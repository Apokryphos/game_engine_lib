#pragma once

#include "engine/debug_gui/entity_info.hpp"
#include "engine/debug_gui/system_debug_panel.hpp"
#include "engine/base_systems/debug_gui_system.hpp"
#include <string>
#include <vector>

namespace engine
{
class Game;
class DebugGuiSystem;

//  ECS system debug GUI panel.
template <typename T>
class EntitySystemDebugPanel : public SystemDebugPanel<T>
{
    std::vector<EntityInfo> m_entity_infos;

protected:
    const std::vector<EntityInfo>& get_entity_infos() const {
        return m_entity_infos;
    }

    //  DebugGuiSystem builds a list of EntityInfo objects for every
    //  entity in the ECS. Here, a filtered list containing only entities
    //  in this specific system is built.
    void build_entity_infos() {
        m_entity_infos.clear();
        get_system_entity_info(
            this->get_system(),
            this->get_debug_gui_system().get_entity_infos(),
            m_entity_infos
        );
    }

public:
    EntitySystemDebugPanel(T& system)
    : SystemDebugPanel<T>(system) {
    }

    virtual void update(Game& game) override {
        build_entity_infos();
        DebugPanel::update(game);
    }
};
}
