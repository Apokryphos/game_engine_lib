#pragma once

#include "engine/base_systems/name_system.hpp"
#include "engine/editor/entity_system_editor_panel.hpp"

namespace engine
{
class NameSystemEditorPanel : public EntitySystemEditorPanel<NameSystem>
{
protected:
    virtual void on_update(Game& game, const ecs::Entity entity) override;

public:
    NameSystemEditorPanel(NameSystem& name_sys);
};
}
