#pragma once

#include "engine/base_systems/name_system.hpp"
#include "engine/debug_gui/debug_gui.hpp"

namespace engine
{
class NameSystemDebugGui : public EntitySystemDebugGui<NameSystem>
{
protected:
    virtual void on_update(Game& game) override;

public:
    NameSystemDebugGui(NameSystem& config_sys);
};
}
