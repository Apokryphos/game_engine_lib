#include "engine/system_manager.hpp"
#include "systems/name_system.hpp"

using namespace engine;

namespace systems
{
//  ----------------------------------------------------------------------------
NameSystem& get_name_system(SystemManager& sys_mgr) {
    return sys_mgr.get_system<NameSystem>(SYSTEM_ID_NAME);
}
}
