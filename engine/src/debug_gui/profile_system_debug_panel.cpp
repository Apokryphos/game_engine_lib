#include "engine/game.hpp"
#include "engine/debug_gui/profile_system_debug_panel.hpp"
#include "imgui.h"

namespace engine
{
//  ----------------------------------------------------------------------------
ProfileSystemDebugPanel::ProfileSystemDebugPanel(ProfileSystem& profile_sys)
: SystemDebugPanel(profile_sys) {
}

//  ----------------------------------------------------------------------------
void ProfileSystemDebugPanel::on_update(Game& game) {
    ProfileSystem& profile_sys = get_system();

    auto* active_profile = profile_sys.get_active_profile();
    if (active_profile != nullptr) {
        ImGui::Text("Active Profile: %s", active_profile->get_name().c_str());
    } else {
        ImGui::Text("No active profile.");
    }
}
}
