#include "systems/editor/position_system_editor_panel.hpp"
#include "systems/position_system.hpp"
#include "imgui.h"

using namespace engine;

namespace systems
{
//  ----------------------------------------------------------------------------
PositionSystemEditorPanel::PositionSystemEditorPanel(PositionSystem& pos_sys)
: EntitySystemEditorPanel(pos_sys) {
}

//  ----------------------------------------------------------------------------
void PositionSystemEditorPanel::on_update(Game& game, const ecs::Entity entity) {
    PositionSystem& pos_sys = get_system();
    const auto name_cmpnt = pos_sys.get_component(entity);
    glm::vec3& position = pos_sys.get_position(name_cmpnt);
    ImGui::InputFloat3("Position", &position.x);
}
}
