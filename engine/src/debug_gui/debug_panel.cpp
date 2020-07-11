#include "engine/debug_gui/debug_panel.hpp"
#include "imgui.h"

namespace engine
{
//  ----------------------------------------------------------------------------
DebugPanel::DebugPanel(const std::string& window_title)
: m_window_title(window_title) {
}

//  ----------------------------------------------------------------------------
void DebugPanel::update(Game& game) {
    on_update(game);
}
}
