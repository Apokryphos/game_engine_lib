#include "engine/base_systems/name_system.hpp"
#include "engine/editor/name_system_editor_panel.hpp"
#include "imgui.h"

namespace engine
{
//  ----------------------------------------------------------------------------
NameSystemEditorPanel::NameSystemEditorPanel(NameSystem& name_sys)
: EntitySystemEditorPanel(name_sys) {
}

//  ----------------------------------------------------------------------------
void NameSystemEditorPanel::on_update(Game& game, const ecs::Entity entity) {
    NameSystem& name_sys = get_system();
    const auto name_cmpnt = name_sys.get_component(entity);
    const std::string& name = name_sys.get_name(name_cmpnt);
    ImGui::Text(name.c_str());
}
}
