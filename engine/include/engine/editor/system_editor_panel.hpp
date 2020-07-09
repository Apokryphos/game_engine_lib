#pragma once

#include "engine/base_systems/debug_gui_system.hpp"
#include "engine/editor/editor_panel.hpp"
#include <string>
#include <vector>

namespace engine
{
class Game;
class DebugGuiSystem;

//  System editor GUI panel.
template <typename T>
class SystemEditorPanel : public EditorPanel
{
    T& m_system;

protected:
    virtual void on_update(Game& game, const ecs::Entity entity) = 0;

public:
    SystemEditorPanel(T& system)
    : EditorPanel(system.get_system_name()),
      m_system(system) {
    }

    T& get_system() {
        return m_system;
    }
};
}
