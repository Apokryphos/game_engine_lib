#pragma once

#include "common/system_id.hpp"
#include <string>
#include <cereal/types/string.hpp>

namespace common
{
class System
{
    using SystemId = common::SystemId;

    SystemId m_id;
    std::string m_name;

public:
    System(const SystemId id, const std::string& name)
    : m_id(id),
      m_name(name) {
    }

    virtual ~System() {}
    System(const System&) = delete;
    System& operator=(const System&) = delete;

    SystemId get_id() const {
        return m_id;
    }

    const std::string& get_system_name() const {
        return m_name;
    }

    template <typename Archive>
    void serialize(Archive& ar) {
        ar(m_id, m_name);
    }
};
}
