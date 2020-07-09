#pragma once

#include "engine/base_systems/config_system.hpp"
#include "engine/debug_gui/debug_gui.hpp"

namespace engine
{
class ConfigSystemDebugGui : public SystemDebugGui<ConfigSystem>
{
protected:
    virtual void on_update(Game& game) override;

public:
    ConfigSystemDebugGui(ConfigSystem& config_sys);
};
}
