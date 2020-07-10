#include "systems/editor/move_system_editor_panel.hpp"
#include "systems/position_system.hpp"
#include "imgui.h"

using namespace engine;

namespace systems
{
//  Starts false.
//  Only changes to true when assigned to a boolean value of true.
//  Use to collect the boolean results of several functions to later
//  test if any change occurred.
class PassBool
{
    bool m_value;

public:
    PassBool()
    : m_value(false) {
    }

    PassBool& operator=(const bool value) {
        if (value) {
            m_value = true;
        }
        return *this;
    }

    explicit operator bool() const {
        return m_value;
    }
};

//  ----------------------------------------------------------------------------
MoveSystemEditorPanel::MoveSystemEditorPanel(MoveSystem& move_sys)
: EntitySystemEditorPanel(move_sys) {
}

//  ----------------------------------------------------------------------------
void MoveSystemEditorPanel::on_update(Game& game, const ecs::Entity entity) {
    MoveSystem& move_sys = get_system();
    const auto move_cmpnt = move_sys.get_component(entity);

    MoveComponentData data;
    move_sys.get_component_data(move_cmpnt, data);

    PassBool modified;
    modified = ImGui::InputFloat("Direction", &data.direction);
    modified = ImGui::InputFloat("Move Speed", &data.move_speed);
    modified = ImGui::InputFloat("Turn Speed", &data.turn_speed);

    if (modified) {
        move_sys.set_component_data(move_cmpnt, data);
    }
}
}
