#pragma once

#include "engine/editor/entity_system_editor_panel.hpp"
#include "systems/move_system.hpp"

namespace systems
{
class MoveSystemEditorPanel : public engine::EntitySystemEditorPanel<MoveSystem>
{
protected:
    virtual void on_update(engine::Game& game, const ecs::Entity entity) override;

public:
    MoveSystemEditorPanel(MoveSystem& move_sys);
};
}
