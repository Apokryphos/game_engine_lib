#pragma once

//  Include base systems util for convenience since there
//  are few base systems.
#include "engine/systems/base_system_util.hpp"

namespace engine
{
class SystemManager;
};

namespace systems
{
class NameSystem;

NameSystem& get_name_system(engine::SystemManager& sys_mgr);
}
