#pragma once

#include "ecs/entity.hpp"
#include "engine/debug_gui/entity_info.hpp"
#include "engine/base_systems/debug_gui_system.hpp"
#include <string>
#include <vector>

namespace engine
{
class Game;
class DebugGuiSystem;

//  Editor GUI panel.
class EditorPanel
{
    friend class DebugGuiSystem;

    std::string m_panel_title;
    DebugGuiSystem* m_debug_gui_sys;

    void set_debug_system(DebugGuiSystem* debug_gui_sys) {
        m_debug_gui_sys = debug_gui_sys;
    }

protected:
    const DebugGuiSystem& get_debug_gui_system() const {
        return *m_debug_gui_sys;
    }

    virtual void on_update(Game& game, const ecs::Entity entity) = 0;

public:
    EditorPanel(const std::string& panel_title);
    virtual ~EditorPanel() {}
    EditorPanel(const EditorPanel&) = delete;
    EditorPanel& operator=(const EditorPanel&) = delete;

    const std::string& get_panel_title() const {
        return m_panel_title;
    }

    virtual void update(Game& game, const ecs::Entity entity);
};

bool begin_panel(const std::string& text);
bool button(const std::string& text);
void end_panel();
}
