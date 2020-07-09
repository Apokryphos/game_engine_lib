#include "engine/game.hpp"
#include "engine/debug_gui/entity_info.hpp"
#include "engine/debug_gui/name_system_debug_panel.hpp"
#include "engine/base_systems/name_system.hpp"
#include "imgui.h"

namespace engine
{
//  ----------------------------------------------------------------------------
NameSystemDebugPanel::NameSystemDebugPanel(NameSystem& name_sys)
: EntitySystemDebugPanel(name_sys) {
}

//  ----------------------------------------------------------------------------
void NameSystemDebugPanel::on_update(Game& game) {
    const auto& entity_infos = get_entity_infos();

    if (entity_infos.empty()) {
        return;
    }

    //  Listbox item getter
    auto entity_getter = [](
        void* vector,
        int index,
        const char** out_text
    ) {
        auto& infos = *static_cast<std::vector<EntityInfo>*>(vector);

        if (index < 0 || index >= static_cast<int>(infos.size())) {
            return false;
        }

        const EntityInfo& info = infos.at(index);
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
        static_cast<void*>(&const_cast<std::vector<EntityInfo>&>(entity_infos)),
        entity_infos.size()
    );
}
}
