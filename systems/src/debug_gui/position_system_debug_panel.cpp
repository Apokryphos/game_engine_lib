#include "engine/game.hpp"
#include "systems/debug_gui/position_system_debug_panel.hpp"
#include "imgui.h"

using namespace engine;

namespace systems
{
//  ----------------------------------------------------------------------------
PositionSystemDebugPanel::PositionSystemDebugPanel(PositionSystem& pos_sys)
: EntitySystemDebugPanel(pos_sys) {
}

//  ----------------------------------------------------------------------------
void PositionSystemDebugPanel::on_update(Game& game) {
    ImGui::Text("Positions.");
}
}
