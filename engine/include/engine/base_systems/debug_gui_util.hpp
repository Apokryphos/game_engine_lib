#pragma once

#include "engine/game.hpp"
#include "engine/base_systems/debug_gui_system.hpp"
#include "engine/base_systems/base_system_util.hpp"
#include "engine/system_manager.hpp"
#include "imgui.h"

namespace engine
{
//  Wraps the passed function, adding a system parameter of the template type.
//  This is kept out of the DebugGuiSystem class to keep things simple and flexible.
template <typename T>
DebugGuiSystem::DebugGuiFunc make_debug_gui_func(
    Game& game,
    const common::SystemId system_id,
    std::function<void (Game& game, T&)> func
) {
    SystemManager& sys_mgr = game.get_system_manager();
    T& system = sys_mgr.get_system<T>(system_id);

    //  Save system name for window title
    const std::string& system_name = system.get_system_name();

    return [func, system_name, &system](Game& game) {
        if (ImGui::Begin(system_name.c_str())) {
            func(game, system);
        }
        ImGui::End();
    };
}

//  Adds a system to the debug GUI system.
//  Wraps the passed function, adding a system parameter of the template type.
template <typename T>
void add_debug_gui(
    Game& game,
    T& system,
    std::function<void (Game& game, T&)> func
) {
    const auto system_id = system.get_id();

    //  Get the debug GUI system
    SystemManager& sys_mgr = game.get_system_manager();
    DebugGuiSystem& debug_gui_sys = get_debug_gui_system(sys_mgr);

    //  Add the system to the debug GUI
    debug_gui_sys.add_gui(
        system_id,
        make_debug_gui_func<T>(game, system_id, func)
    );
}
}
