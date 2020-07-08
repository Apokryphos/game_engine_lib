#pragma once

namespace engine
{
class SystemManager;
};

namespace systems
{
class NameSystem;

NameSystem& get_name_system(engine::SystemManager& sys_mgr);
}
