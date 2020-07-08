#pragma once

namespace engine
{
class Game;
class SystemManager;
};

namespace systems
{
class ConfigSystem;
class NameSystem;
class ProfileSystem;

ConfigSystem& get_config_system(engine::SystemManager& sys_mgr);
NameSystem& get_name_system(engine::SystemManager& sys_mgr);
ProfileSystem& get_profile_system(engine::SystemManager& sys_mgr);
}
