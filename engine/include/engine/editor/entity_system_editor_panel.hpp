#pragma once

#include "engine/editor/system_editor_panel.hpp"
#include "engine/base_systems/editor_system.hpp"
#include <string>
#include <vector>

namespace engine
{
class Game;
class EditorSystem;

//  ECS system editor GUI panel.
template <typename T>
class EntitySystemEditorPanel : public SystemEditorPanel<T>
{
public:
    EntitySystemEditorPanel(T& system)
    : SystemEditorPanel<T>(system) {
    }

    virtual void update(Game& game, const ecs::Entity entity) override {
        if (!begin_panel(this->get_panel_title())) {
            end_panel();
            return;
        }

        if (!this->get_system().has_component(entity)) {
            if (button("Add Component")) {
                this->get_system().add_component(entity);
            }
        } else {
            this->on_update(game, entity);
        }

        end_panel();
    }
};
}
