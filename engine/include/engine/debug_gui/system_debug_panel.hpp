#pragma once

#include "engine/debug_gui/debug_panel.hpp"
#include "engine/debug_gui/entity_info.hpp"
#include "engine/base_systems/debug_gui_system.hpp"
#include <string>
#include <vector>

namespace engine
{
class Game;
class DebugGuiSystem;

//  System debug GUI panel.
template <typename T>
class SystemDebugPanel : public DebugPanel
{
    T& m_system;

public:
    SystemDebugPanel(T& system)
    : DebugPanel(system.get_system_name()),
      m_system(system) {
    }

    T& get_system() {
        return m_system;
    }
};
}
