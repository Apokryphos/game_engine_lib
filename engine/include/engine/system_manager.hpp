#pragma once

#include "common/system.hpp"
#include <algorithm>
#include <memory>
#include <vector>

namespace ecs
{
class EcsRoot;
}

namespace engine
{
class SystemManager
{
    using EcsRoot = ecs::EcsRoot;
    using System = common::System;
    using SystemId = common::SystemId;

    EcsRoot& m_ecs_root;
    std::vector<std::unique_ptr<System>> m_systems;

public:
    SystemManager(EcsRoot& ecs_root);
    void add_system(std::unique_ptr<System> system);

    template <typename T>
    T& get_system(const SystemId id) {
        auto find = std::find_if(
            m_systems.begin(),
            m_systems.end(),
            [id](const auto& system) {
                return system->get_id() == id;
            }
        );

        if (find == m_systems.end()) {
            throw std::runtime_error("System not found.");
        }

        System* system = (*find).get();
        return *(static_cast<T*>(system));
    }

    template <typename Archive>
    void serialize(Archive& ar) {
        ar(
            m_systems
        );
    }
};
}
