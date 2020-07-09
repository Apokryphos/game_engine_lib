#include "engine/game.hpp"
#include "systems/name_system.hpp"
#include "imgui.h"

using namespace engine;

namespace systems
{
//  ----------------------------------------------------------------------------
void name_system_debug_gui(Game& game, NameSystem& name_sys) {
    ImGui::Text("Names.");
}
}
