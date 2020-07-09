#include "common/log.hpp"
#include "demo/input/input_action_ids.hpp"
#include "demo/systems/demo_system.hpp"
#include "demo/ui/demo_state.hpp"
#include "engine/base_systems/base_system_util.hpp"
#include "engine/base_systems/debug_gui_system.hpp"
#include "engine/base_systems/editor_system.hpp"
#include "engine/engine.hpp"
#include "engine/game.hpp"
#include "engine/system_manager.hpp"
#include "input/input_device.hpp"
#include "input/input_event.hpp"
#include "input/input_manager.hpp"
#include <glm/glm.hpp>

using namespace common;
using namespace engine;
using namespace input;

namespace demo
{
static const bool LOG_EVENTS = true;

//  ----------------------------------------------------------------------------
void log_event(const InputEvent& event, const InputDevice& device) {
    if (!LOG_EVENTS) {
        return;
    }

    const InputActionId action_id = event.get_action_id();

    const InputActionStringEntry* entry = get_input_action_string(action_id);

    log_info(
        "%s (%s) (Analog: %.2f) (Digital: %s) (Device %d: %s)",
        entry->name.c_str(),
        to_string(event.get_type()).c_str(),
        event.get_analog_value(),
        event.get_digital_value() ? "ON" : "OFF",
        device.get_id(),
        device.get_name().c_str()
    );
}

//  ----------------------------------------------------------------------------
void DemoState::on_process_event(Game& game, const InputEvent& event) {
    Engine& engine = game.get_engine();
    InputManager& input_mgr = engine.get_input_manager();
    InputDevice& device = input_mgr.get_device(event.get_device_id());

    const InputActionId action_id = event.get_action_id();

    SystemManager& sys_mgr = game.get_system_manager();
    DemoSystem& demo_sys = sys_mgr.get_system<DemoSystem>(SYSTEM_ID_DEMO);

    log_event(event, device);

    //  Check for events only on activate (press)
    switch (event.get_action_id()) {
        default:
            log_debug("DemoState::on_process_event ignored event.");
            break;

        case INPUT_ACTION_ID_TOGGLE_DEBUG_GUI: {
            DebugGuiSystem& debug_gui_sys = get_debug_gui_system(sys_mgr);
            debug_gui_sys.toggle_visible();

            EditorSystem& editor_sys = get_editor_system(sys_mgr);
            editor_sys.toggle_visible();
            break;
        }

        case INPUT_ACTION_ID_QUIT:
            game.quit();
            break;

        case INPUT_ACTION_ID_ACCEPT:
            break;

        case INPUT_ACTION_ID_CANCEL:
            break;

        case INPUT_ACTION_ID_MOVE_DOWN:
            demo_sys.forward(0.1f);
            break;

        case INPUT_ACTION_ID_MOVE_UP:
            demo_sys.forward(-0.1f);
            break;

        case INPUT_ACTION_ID_MOVE_RIGHT:
            demo_sys.strafe(0.1f);
            break;

        case INPUT_ACTION_ID_MOVE_LEFT:
            demo_sys.strafe(-0.1f);
            break;

        case INPUT_ACTION_ID_AIM_HORZ:
            break;

        case INPUT_ACTION_ID_AIM_VERT:
            break;

        case INPUT_ACTION_ID_FIRE:
            break;

        case INPUT_ACTION_ID_ROTATE_CW:
            demo_sys.rotate(glm::radians(event.get_analog_value() * -2.0f));
            break;

        case INPUT_ACTION_ID_ROTATE_CCW:
            demo_sys.rotate(glm::radians(event.get_analog_value() * 2.0f));
            break;

        case INPUT_ACTION_ID_ZOOM_IN:
            demo_sys.zoom(-1.0f);
            break;

        case INPUT_ACTION_ID_ZOOM_OUT:
            demo_sys.zoom(1.0f);
            break;
    }
}
}
