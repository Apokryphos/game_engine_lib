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

    const size_t MAX_NAME_LEN = 64;
    std::array<char, MAX_NAME_LEN> str_array;
    str_array.fill(0);
    std::copy(name.begin(), name.end(), str_array.data());
    // name.copy(str_array.data(), std::min(name.length()+1, MAX_NAME_LEN));
    if (ImGui::InputText("Name", str_array.data(), str_array.size())) {
        const std::string new_name = str_array.data();
        name_sys.set_name(name_cmpnt, new_name);
    }
}
}
