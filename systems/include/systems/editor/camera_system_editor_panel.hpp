#pragma once

#include "engine/editor/entity_system_editor_panel.hpp"
#include "systems/camera_system.hpp"

namespace systems
{
class CameraSystemEditorPanel : public engine::EntitySystemEditorPanel<CameraSystem>
{
protected:
    virtual void on_update(engine::Game& game, const ecs::Entity entity) override;

public:
    CameraSystemEditorPanel(CameraSystem& camera_sys);
};
}
