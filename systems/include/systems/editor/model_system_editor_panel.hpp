#pragma once

#include "engine/editor/entity_system_editor_panel.hpp"
#include "systems/model_system.hpp"

namespace systems
{
class ModelSystemEditorPanel : public engine::EntitySystemEditorPanel<ModelSystem>
{
protected:
    virtual void on_update(engine::Game& game, const ecs::Entity entity) override;

public:
    ModelSystemEditorPanel(ModelSystem& model_sys);
};
}
