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
    if (!ImGui::Begin(m_window_title.c_str())) {
        ImGui::End();
    }

    on_update(game);

    ImGui::End();
}
}
