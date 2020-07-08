#pragma once

namespace engine
{
class ConfigSystem;
class ProfileSystem;
class SystemManager;

ConfigSystem& get_config_system(SystemManager& sys_mgr);
ProfileSystem& get_profile_system(SystemManager& sys_mgr);
}
