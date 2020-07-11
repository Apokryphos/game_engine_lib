#include "engine/editor/editor_panel.hpp"
#include "imgui.h"

using namespace ecs;

namespace engine
{
//  ----------------------------------------------------------------------------
bool button(const std::string& text) {
    return ImGui::Button(text.c_str());
}

//  ----------------------------------------------------------------------------
EditorPanel::EditorPanel(const std::string& panel_title)
: m_panel_title(panel_title) {
}

//  ----------------------------------------------------------------------------
void EditorPanel::update(Game& game, const Entity entity) {
    on_update(game, entity);
}
}
