#include "engine/game.hpp"
#include "engine/debug_gui/config_system_debug_gui.hpp"
#include "imgui.h"

namespace engine
{
//  ----------------------------------------------------------------------------
ConfigSystemDebugGui::ConfigSystemDebugGui(ConfigSystem& config_sys)
: SystemDebugGui(config_sys) {
}

//  ----------------------------------------------------------------------------
void ConfigSystemDebugGui::on_update(Game& game) {
    ImGui::Text("Config!");
}
}
