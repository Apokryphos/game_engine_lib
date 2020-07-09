#pragma once

#include "engine/base_systems/name_system.hpp"
#include "engine/debug_gui/entity_system_debug_panel.hpp"

namespace engine
{
class NameSystemDebugPanel : public EntitySystemDebugPanel<NameSystem>
{
protected:
    virtual void on_update(Game& game) override;

public:
    NameSystemDebugPanel(NameSystem& name_sys);
};
}
