#include "engine/game.hpp"
#include "engine/debug_gui/debug_gui_system_debug_panel.hpp"
#include "imgui.h"

namespace engine
{
//  ----------------------------------------------------------------------------
DebugGuiSystemDebugPanel::DebugGuiSystemDebugPanel(DebugGuiSystem& debug_gui_sys)
: SystemDebugPanel(debug_gui_sys) {
}

//  ----------------------------------------------------------------------------
void DebugGuiSystemDebugPanel::on_update(Game& game) {
    #ifdef DEBUG
    ImGui::Text("Debug Build");
    #else
    ImGui::Text("Release Build");
    #endif

    ImGui::Text(
        "Average %.3f ms/frame (%.1f FPS)",
        1000.0f / ImGui::GetIO().Framerate,
        ImGui::GetIO().Framerate
    );
}
}
