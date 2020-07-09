#include "engine/editor/editor_panel.hpp"
#include "imgui.h"

using namespace ecs;

namespace engine
{
//  ----------------------------------------------------------------------------
bool begin_panel(const std::string& text) {
    return ImGui::CollapsingHeader(text.c_str(), ImGuiTreeNodeFlags_DefaultOpen);
}

//  ----------------------------------------------------------------------------
bool button(const std::string& text) {
    return ImGui::Button(text.c_str());
}

//  ----------------------------------------------------------------------------
void end_panel() {
    //  Collapsing header doesn't require ImGui::End()
    // ImGui::End();
}

//  ----------------------------------------------------------------------------
EditorPanel::EditorPanel(const std::string& panel_title)
: m_panel_title(panel_title) {
}

//  ----------------------------------------------------------------------------
void EditorPanel::update(Game& game, const Entity entity) {
    if (!begin_panel(m_panel_title)) {
        end_panel();
        return;
    }

    on_update(game, entity);

    end_panel();
}
}
