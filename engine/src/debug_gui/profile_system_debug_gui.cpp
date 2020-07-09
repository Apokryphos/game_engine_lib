#include "engine/game.hpp"
#include "engine/base_systems/profile_system.hpp"
#include "imgui.h"

namespace engine
{
//  ----------------------------------------------------------------------------
void profile_system_debug_gui(Game& game, ProfileSystem& profile_sys) {
    auto* active_profile = profile_sys.get_active_profile();
    if (active_profile != nullptr) {
        ImGui::Text("Active Profile: %s", active_profile->get_name().c_str());
    } else {
        ImGui::Text("No active profile.");
    }
}
}
