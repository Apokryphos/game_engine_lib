#include "engine/base_systems/debug_gui_system.hpp"
#include "engine/debug_gui/debug_panel.hpp"
#include "engine/debug_gui/entity_info.hpp"
#include "engine/game.hpp"
#include "engine/system_manager.hpp"
#include "imgui.h"

using namespace common;

namespace engine
{
//  ----------------------------------------------------------------------------
DebugGuiSystem::DebugGuiSystem()
: System(DebugGuiSystem::Id, "debug_gui_system"),
  m_visible(false) {
}

//  ----------------------------------------------------------------------------
void DebugGuiSystem::add_gui(std::unique_ptr<DebugPanel> panel) {
    if (panel == nullptr) {
        throw std::runtime_error("Debug panel cannot be null.");
    }

    Entry entry{};
    entry.title = panel->get_window_title();
    entry.panel = panel.get();

    m_entries.push_back(entry);

    panel->set_debug_system(this);
    m_panels.push_back(std::move(panel));
}

//  ----------------------------------------------------------------------------
void DebugGuiSystem::add_gui(const std::string& title, DebugGuiFunc func) {
    if (!func) {
        throw std::runtime_error("Debug function cannot be null.");
    }

    Entry entry{};
    entry.title = title;
    entry.func = func;

    m_entries.push_back(entry);
}

//  ----------------------------------------------------------------------------
const std::vector<EntityInfo>& DebugGuiSystem::get_entity_infos() const {
    return m_entity_infos;
}

//  ----------------------------------------------------------------------------
void DebugGuiSystem::toggle_visible() {
    m_visible = !m_visible;
}

//  ----------------------------------------------------------------------------
void DebugGuiSystem::update_system_manager_gui(Game& game) {
    ImGui::Begin("system_manager");

    SystemManager& sys_mgr = game.get_system_manager();

    //  Allow visibility of system debug GUI windows to be toggled
    for (Entry& entry : m_entries) {
        //  Checkbox to toggle Window visibility
        ImGui::Checkbox(entry.title.c_str(), &entry.visible);
    }

    ImGui::End();
}

//  ----------------------------------------------------------------------------
void DebugGuiSystem::update(Game& game) {
    if (!m_visible) {
        return;
    }

    //  Build entity debug infos
    m_entity_infos.clear();
    build_entity_infos(game, m_entity_infos);

    update_system_manager_gui(game);

    //  Update debug panels
    for (const Entry& entry : m_entries) {
        if (!entry.visible) {
            continue;
        }

        if (ImGui::Begin(entry.title.c_str())) {
            if (entry.panel) {
                entry.panel->update(game);
            }

            if (entry.func) {
                entry.func(game);
            }
        }

        ImGui::End();
    }
}
}
