#include "engine/base_systems/debug_gui_system.hpp"
#include "engine/debug_gui/debug_gui.hpp"
#include "engine/debug_gui/entity_debug_info.hpp"
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
void DebugGuiSystem::add_gui(std::unique_ptr<DebugGui> debug_gui) {
    if (debug_gui == nullptr) {
        throw std::runtime_error("Debug GUI cannot be null.");
    }

    Entry entry{};
    entry.title = debug_gui->get_window_title();
    entry.debug_gui = debug_gui.get();

    m_entries.push_back(entry);

    debug_gui->set_debug_system(this);
    m_debug_gui.push_back(std::move(debug_gui));
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
const std::vector<EntityDebugInfo>& DebugGuiSystem::get_entity_debug_infos() const {
    return m_entity_debug_infos;
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
    m_entity_debug_infos.clear();
    build_entity_debug_infos(game, m_entity_debug_infos);

    update_system_manager_gui(game);

    //  Update debug GUIs
    for (const Entry& entry : m_entries) {
        if (!entry.visible) {
            continue;
        }

        if (entry.debug_gui) {
            entry.debug_gui->update(game);
        }

        if (entry.func) {
            entry.func(game);
        }
    }
}
}
