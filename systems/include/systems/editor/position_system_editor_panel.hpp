#pragma once

#include "engine/editor/entity_system_editor_panel.hpp"
#include "systems/position_system.hpp"

namespace systems
{
class PositionSystemEditorPanel : public engine::EntitySystemEditorPanel<PositionSystem>
{
protected:
    virtual void on_update(engine::Game& game, const ecs::Entity entity) override;

public:
    PositionSystemEditorPanel(PositionSystem& pos_sys);
};
}
