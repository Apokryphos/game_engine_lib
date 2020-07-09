#include "engine/game.hpp"
#include "engine/base_systems/config_system.hpp"
#include "imgui.h"

namespace engine
{
//  ----------------------------------------------------------------------------
void config_system_debug_gui(Game& game, ConfigSystem& config_sys) {
    ImGui::Text("Config!");
}
}
