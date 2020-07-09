#pragma once

#include "engine/debug_gui/entity_debug_info.hpp"
#include "engine/base_systems/debug_gui_system.hpp"
#include <string>
#include <vector>

namespace engine
{
class Game;
class DebugGuiSystem;

//  Debug GUI window.
class DebugPanel
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
    DebugPanel(const std::string& window_title);
    virtual ~DebugPanel() {}
    DebugPanel(const DebugPanel&) = delete;
    DebugPanel& operator=(const DebugPanel&) = delete;

    const std::string& get_window_title() const {
        return m_window_title;
    }

    virtual void update(Game& game);
};
}
