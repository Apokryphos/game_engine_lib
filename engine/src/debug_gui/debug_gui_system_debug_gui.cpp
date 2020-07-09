#include "engine/game.hpp"
#include "engine/base_systems/debug_gui_system.hpp"
#include "imgui.h"

namespace engine
{
//  ----------------------------------------------------------------------------
void debug_gui_system_debug_gui(Game& game, DebugGuiSystem& debug_gui_sys) {
    ImGui::Text(
        "Average %.3f ms/frame (%.1f FPS)",
        1000.0f / ImGui::GetIO().Framerate,
        ImGui::GetIO().Framerate
    );
}
}
