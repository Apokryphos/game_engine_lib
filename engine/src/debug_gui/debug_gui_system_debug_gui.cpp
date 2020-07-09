#include "engine/game.hpp"
#include "engine/debug_gui/debug_gui_system_debug_gui.hpp"
#include "imgui.h"

namespace engine
{
//  ----------------------------------------------------------------------------
DebugGuiSystemDebugGui::DebugGuiSystemDebugGui(DebugGuiSystem& debug_gui_sys)
: SystemDebugGui(debug_gui_sys) {
}

//  ----------------------------------------------------------------------------
void DebugGuiSystemDebugGui::on_update(Game& game) {
    ImGui::Text(
        "Average %.3f ms/frame (%.1f FPS)",
        1000.0f / ImGui::GetIO().Framerate,
        ImGui::GetIO().Framerate
    );
}
}
