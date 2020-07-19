#pragma once

#include "ecs/entity.hpp"
#include <string>

namespace engine
{
class ConfigSystem;
class Game;
class DebugGuiSystem;
class EditorSystem;
class NameSystem;
class ProfileSystem;
class SystemManager;

void add_name_component(
    const ecs::Entity entity,
    NameSystem& name_sys,
    const std::string& name
);

ConfigSystem& get_config_system(SystemManager& sys_mgr);
DebugGuiSystem& get_debug_gui_system(SystemManager& sys_mgr);
EditorSystem& get_editor_system(engine::SystemManager& sys_mgr);
NameSystem& get_name_system(engine::SystemManager& sys_mgr);
ProfileSystem& get_profile_system(SystemManager& sys_mgr);

void initialize_base_systems(
    Game& game,
    const std::string& game_base_name,
    const size_t component_count = 10000
);
}
