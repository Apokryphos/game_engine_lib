#pragma once

#include "engine/debug_gui/entity_debug_info.hpp"
#include "engine/base_systems/debug_gui_system.hpp"
#include <string>
#include <vector>

namespace engine
{
class Game;
class DebugGuiSystem;

class DebugGui
{
    friend class DebugGuiSystem;

    std::string m_window_title;
    DebugGuiSystem* m_debug_gui_sys;

    void set_debug_system(DebugGuiSystem* debug_gui_sys) {
        m_debug_gui_sys = debug_gui_sys;
    }

protected:
    const DebugGuiSystem& get_debug_gui_system() const {
        return *m_debug_gui_sys;
    }

    virtual void on_update(Game& game) = 0;

public:
    DebugGui(const std::string& window_title);
    virtual ~DebugGui() {}
    DebugGui(const DebugGui&) = delete;
    DebugGui& operator=(const DebugGui&) = delete;

    const std::string& get_window_title() const {
        return m_window_title;
    }

    virtual void update(Game& game);
};

//  System debug GUI
template <typename T>
class SystemDebugGui : public DebugGui
{
    T& m_system;

public:
    SystemDebugGui(T& system)
    : DebugGui(system.get_system_name()),
      m_system(system) {
    }

    T& get_system() {
        return m_system;
    }
};

//  ECS debug GUI
template <typename T>
class EntitySystemDebugGui : public SystemDebugGui<T>
{
    std::vector<EntityDebugInfo> m_entity_debug_infos;

protected:
    const std::vector<EntityDebugInfo>& get_entity_debug_infos() const {
        return m_entity_debug_infos;
    }

    //  DebugGuiSystem builds a list of EntityDebugInfo objects for every
    //  entity in the ECS. Here, a filtered list containing only entities
    //  in this specific system is built.
    void build_entity_debug_infos() {
        get_system_entity_debug_info(
            this->get_system(),
            this->get_debug_gui_system().get_entity_debug_infos(),
            m_entity_debug_infos
        );
    }

public:
    EntitySystemDebugGui(T& system)
    : SystemDebugGui<T>(system) {
    }

    virtual void update(Game& game) override {
        build_entity_debug_infos();
        DebugGui::update(game);
    }
};
}
