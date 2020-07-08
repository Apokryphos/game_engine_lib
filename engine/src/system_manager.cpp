#include "common/log.hpp"
#include "engine/system_manager.hpp"
#include "ecs/ecs_root.hpp"
#include "ecs/entity_system_base.hpp"

using namespace common;
using namespace ecs;

namespace engine
{
//  ----------------------------------------------------------------------------
SystemManager::SystemManager(EcsRoot& ecs_root)
: m_ecs_root(ecs_root) {
}

//  ----------------------------------------------------------------------------
void SystemManager::add_system(std::unique_ptr<System> system) {
    const auto& find = std::find_if(
        m_systems.begin(),
        m_systems.end(),
        [&system](const auto& sys) {
            return sys->get_id() == system->get_id();
        }
    );

    if (find != m_systems.end()) {
        throw std::runtime_error("A system with the same ID was already added.");
    }

    System* sys = system.get();

    m_systems.push_back(std::move(system));

    EntitySystemBase* ecs_sys = dynamic_cast<EntitySystemBase*>(sys);
    if (ecs_sys != nullptr) {
        m_ecs_root.add_system(ecs_sys);
        log_debug("Added ECS system '%s'.", ecs_sys->get_system_name().c_str());
    } else {
        log_debug("Added system '%s'.", sys->get_system_name().c_str());
    }
}
}
