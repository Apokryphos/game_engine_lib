#include "engine/game.hpp"
#include "engine/debug_gui/entity_debug_info.hpp"
#include "engine/debug_gui/name_system_debug_gui.hpp"
#include "engine/base_systems/name_system.hpp"
#include "imgui.h"

namespace engine
{
//  ----------------------------------------------------------------------------
NameSystemDebugGui::NameSystemDebugGui(NameSystem& name_sys)
: EntitySystemDebugGui(name_sys) {
}

//  ----------------------------------------------------------------------------
void NameSystemDebugGui::on_update(Game& game) {
    const auto& entity_debug_infos = get_entity_debug_infos();

    if (entity_debug_infos.empty()) {
        return;
    }

    //  Listbox item getter
    auto entity_getter = [](
        void* vector,
        int index,
        const char** out_text
    ) {
        auto& infos = *static_cast<std::vector<EntityDebugInfo>*>(vector);

        if (index < 0 || index >= static_cast<int>(infos.size())) {
            return false;
        }

        const EntityDebugInfo& info = infos.at(index);
        *out_text = info.name.c_str();
        return true;
    };

    //  Listbox
    static int index = 0;
    ImGui::Text("Entities");
    ImGui::PushItemWidth(-1);
    ImGui::ListBox(
        "Entities",
        &index,
        entity_getter,
        static_cast<void*>(&const_cast<std::vector<EntityDebugInfo>&>(entity_debug_infos)),
        entity_debug_infos.size()
    );
}
}
