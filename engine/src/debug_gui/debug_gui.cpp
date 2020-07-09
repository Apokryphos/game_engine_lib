#include "engine/debug_gui/debug_gui.hpp"
#include "imgui.h"

namespace engine
{
//  ----------------------------------------------------------------------------
DebugGui::DebugGui(const std::string& window_title)
: m_window_title(window_title) {
}

//  ----------------------------------------------------------------------------
void DebugGui::update(Game& game) {
    if (!ImGui::Begin(m_window_title.c_str())) {
        ImGui::End();
    }

    on_update(game);

    ImGui::End();
}
}
