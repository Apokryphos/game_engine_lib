#pragma once

#include "engine/base_systems/config_system.hpp"
#include "engine/debug_gui/system_debug_panel.hpp"

namespace engine
{
class ConfigSystemDebugPanel : public SystemDebugPanel<ConfigSystem>
{
protected:
    virtual void on_update(Game& game) override;

public:
    ConfigSystemDebugPanel(ConfigSystem& config_sys);
};
}
