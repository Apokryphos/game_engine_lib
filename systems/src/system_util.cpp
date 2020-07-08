#include "engine/system_manager.hpp"
#include "systems/config_system.hpp"
#include "systems/name_system.hpp"
#include "systems/profile_system.hpp"

using namespace engine;

namespace systems
{
//  ----------------------------------------------------------------------------
ConfigSystem& get_config_system(SystemManager& sys_mgr) {
    return sys_mgr.get_system<ConfigSystem>(SYSTEM_ID_CONFIG);
}

//  ----------------------------------------------------------------------------
NameSystem& get_name_system(SystemManager& sys_mgr) {
    return sys_mgr.get_system<NameSystem>(SYSTEM_ID_NAME);
}

//  ----------------------------------------------------------------------------
ProfileSystem& get_profile_system(SystemManager& sys_mgr) {
    return sys_mgr.get_system<ProfileSystem>(SYSTEM_ID_PROFILE);
}
}
