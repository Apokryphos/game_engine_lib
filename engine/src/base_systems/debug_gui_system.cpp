#include "engine/base_systems/debug_gui_system.hpp"
#include "engine/game.hpp"
#include "engine/system_manager.hpp"
#include "imgui.h"

using namespace common;

namespace engine
{
//  ----------------------------------------------------------------------------
DebugGuiSystem::DebugGuiSystem()
: System(DebugGuiSystem::Id, "debug_gui_system") {
}

//  ----------------------------------------------------------------------------
void DebugGuiSystem::add_gui(const SystemId system_id, DebugGuiFunc func) {
    if (m_entries.find(system_id) != m_entries.end()) {
        throw std::runtime_error("Only one debug GUI function per system can be added.");
    }

    Entry entry{};
    entry.visible = true;
    entry.func = func;

    m_entries[system_id] = entry;
}

//  ----------------------------------------------------------------------------
void DebugGuiSystem::update_system_manager_gui(Game& game) {
    ImGui::Begin("system_manager");

    SystemManager& sys_mgr = game.get_system_manager();

    //  Allow visibility of system debug GUI windows to be toggled
    for (auto& pair : m_entries) {
        const SystemId sys_id = pair.first;
        DebugGuiSystem::Entry& entry = pair.second;

        const System& sys = sys_mgr.get_system<System>(sys_id);
        const std::string sys_name = sys.get_system_name();

        //  Checkbox to toggle Window visibility
        ImGui::Checkbox(sys_name.c_str(), &entry.visible);
    }

    ImGui::End();
}

//  ----------------------------------------------------------------------------
void DebugGuiSystem::update(Game& game) {
    update_system_manager_gui(game);

    for (const auto& pair : m_entries) {
        const DebugGuiSystem::Entry& entry = pair.second;

        if (!entry.visible) {
            continue;
        }

        if (entry.func) {
            entry.func(game);
        }
    }
}
}
