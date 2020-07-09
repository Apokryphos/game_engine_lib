#include "engine/game.hpp"
#include "engine/base_systems/profile_system.hpp"
#include "imgui.h"

namespace engine
{
//  ----------------------------------------------------------------------------
void profile_system_debug_gui(Game& game, ProfileSystem& profile_sys) {
    ImGui::Text("Profiles.");
}
}
