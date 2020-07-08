#pragma once

#include <string>

namespace engine
{
class ConfigSystem;
class DebugGuiSystem;
class ProfileSystem;
class SystemManager;

ConfigSystem& get_config_system(SystemManager& sys_mgr);
DebugGuiSystem& get_debug_gui_system(SystemManager& sys_mgr);
ProfileSystem& get_profile_system(SystemManager& sys_mgr);

void initialize_base_systems(
    SystemManager& sys_mgr,
    const std::string& game_base_name
);
}
