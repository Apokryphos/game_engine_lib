#pragma once

//  Include base systems util for convenience since there
//  are few base systems.
#include "engine/base_systems/base_system_util.hpp"

namespace engine
{
class SystemManager;
};

namespace systems
{
class PositionSystem;

PositionSystem& get_position_system(engine::SystemManager& sys_mgr);
}
