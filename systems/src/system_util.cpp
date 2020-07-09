#include "engine/system_manager.hpp"
#include "systems/position_system.hpp"

using namespace engine;

namespace systems
{
//  ----------------------------------------------------------------------------
PositionSystem& get_position_system(engine::SystemManager& sys_mgr) {
    return sys_mgr.get_system<PositionSystem>(SYSTEM_ID_POSITION);
}
}
