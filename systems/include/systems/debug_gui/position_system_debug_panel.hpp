#pragma once

#include "engine/debug_gui/entity_system_debug_panel.hpp"
#include "systems/position_system.hpp"

namespace systems
{
class PositionSystemDebugPanel : public engine::EntitySystemDebugPanel<PositionSystem>
{
protected:
    virtual void on_update(engine::Game& game) override;

public:
    PositionSystemDebugPanel(PositionSystem& pos_sys);
};
}
