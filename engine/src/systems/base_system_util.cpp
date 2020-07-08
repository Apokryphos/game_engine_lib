#include "engine/system_manager.hpp"
#include "engine/systems/base_system_util.hpp"
#include "engine/systems/config_system.hpp"
#include "engine/systems/debug_gui_system.hpp"
#include "engine/systems/profile_system.hpp"

#include "imgui.h"

namespace engine
{
//  ----------------------------------------------------------------------------
static void config_system_debug_gui(Game& game) {
    ImGui::Begin("ConfigSystem");
    ImGui::Text("Config!");
    ImGui::End();
}

//  ----------------------------------------------------------------------------
ConfigSystem& get_config_system(SystemManager& sys_mgr) {
    return sys_mgr.get_system<ConfigSystem>(SYSTEM_ID_CONFIG);
}

//  ----------------------------------------------------------------------------
DebugGuiSystem& get_debug_gui_system(SystemManager& sys_mgr) {
    return sys_mgr.get_system<DebugGuiSystem>(SYSTEM_ID_DEBUG_GUI);
}

//  ----------------------------------------------------------------------------
ProfileSystem& get_profile_system(SystemManager& sys_mgr) {
    return sys_mgr.get_system<ProfileSystem>(SYSTEM_ID_PROFILE);
}

//  ----------------------------------------------------------------------------
void initialize_base_systems(
    SystemManager& sys_mgr,
    const std::string& game_base_name
) {
    //  Debug GUI
    sys_mgr.add_system(std::make_unique<DebugGuiSystem>());
    DebugGuiSystem& debug_gui_sys = sys_mgr.get_system<DebugGuiSystem>(SYSTEM_ID_DEBUG_GUI);

    //  Configuration system
    sys_mgr.add_system(std::make_unique<ConfigSystem>(game_base_name));
    ConfigSystem& config_sys = get_config_system(sys_mgr);
    debug_gui_sys.add_gui(SYSTEM_ID_CONFIG, config_system_debug_gui);

    //  Profile system
    sys_mgr.add_system(std::make_unique<ProfileSystem>(game_base_name));
}
}
