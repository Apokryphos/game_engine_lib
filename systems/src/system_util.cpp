#include "engine/system_manager.hpp"
#include "systems/name_system.hpp"
#include "systems/position_system.hpp"

using namespace engine;

namespace systems
{
//  ----------------------------------------------------------------------------
NameSystem& get_name_system(SystemManager& sys_mgr) {
    return sys_mgr.get_system<NameSystem>(SYSTEM_ID_NAME);
}

//  ----------------------------------------------------------------------------
PositionSystem& get_position_system(engine::SystemManager& sys_mgr) {
    return sys_mgr.get_system<PositionSystem>(SYSTEM_ID_POSITION);
}

}
