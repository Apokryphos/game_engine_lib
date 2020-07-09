#include "engine/game.hpp"
#include "engine/debug_gui/config_system_debug_panel.hpp"
#include "imgui.h"

namespace engine
{
//  ----------------------------------------------------------------------------
ConfigSystemDebugPanel::ConfigSystemDebugPanel(ConfigSystem& config_sys)
: SystemDebugPanel(config_sys) {
}

//  ----------------------------------------------------------------------------
void ConfigSystemDebugPanel::on_update(Game& game) {
    ImGui::Text("Config!");
}
}
