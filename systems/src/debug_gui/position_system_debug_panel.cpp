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
