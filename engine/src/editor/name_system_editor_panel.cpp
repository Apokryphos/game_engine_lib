#include "engine/base_systems/name_system.hpp"
#include "engine/editor/name_system_editor_panel.hpp"
#include "imgui.h"
#include <array>

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

    const unsigned MAX_NAME_LEN = 64;
    std::array<char, MAX_NAME_LEN> str_array;
    name.copy(str_array.data(), MAX_NAME_LEN);
    ImGui::InputText("Name", str_array.data(), MAX_NAME_LEN);
    const std::string& new_name = str_array.data();
    name_sys.set_name(name_cmpnt, new_name.data());
}
}
