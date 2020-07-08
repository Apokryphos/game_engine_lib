#include "common/log.hpp"
#include "demo/input/input_action_ids.hpp"
#include "demo/systems/demo_system.hpp"
#include "demo/ui/demo_state.hpp"
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
static const bool LOG_EVENTS = false;

//  ----------------------------------------------------------------------------
void log_event(
    const std::string& text,
    const InputEvent& event,
    const InputDevice& device
) {
    if (!LOG_EVENTS) {
        return;
    }

    const InputActionId action_id = event.get_action_id();

    log_info(
        "%s (%s) (Analog: %.2f) (Digital: %s) (Device %d: %s)",
        text.c_str(),
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

    //  Check for events only on activate (press)
    switch (event.get_action_id()) {
        case INPUT_ACTION_ID_QUIT:
            log_event("QUIT", event, device);
            game.quit();
            break;

        case INPUT_ACTION_ID_ACCEPT:
            log_event("ACCEPT", event, device);
            break;

        case INPUT_ACTION_ID_CANCEL:
            log_event("CANCEL", event, device);
            break;

        case INPUT_ACTION_ID_MOVE_DOWN:
            log_event("MOVE DOWN", event, device);
            demo_sys.forward(0.1f);
            break;

        case INPUT_ACTION_ID_MOVE_UP:
            log_event("MOVE UP", event, device);
            demo_sys.forward(-0.1f);
            break;

        case INPUT_ACTION_ID_MOVE_RIGHT:
            log_event("MOVE RIGHT", event, device);
            demo_sys.strafe(0.1f);
            break;

        case INPUT_ACTION_ID_MOVE_LEFT:
            log_event("MOVE LEFT", event, device);
            demo_sys.strafe(-0.1f);
            break;

        case INPUT_ACTION_ID_AIM_HORZ:
            log_event("AIM X", event, device);
            break;

        case INPUT_ACTION_ID_AIM_VERT:
            log_event("AIM Y", event, device);
            break;

        case INPUT_ACTION_ID_FIRE:
            log_event("FIRE", event, device);
            break;

        case INPUT_ACTION_ID_ROTATE_CW:
            log_event("ROTATE CW", event, device);
            demo_sys.rotate(glm::radians(event.get_analog_value() * -2.0f));
            break;

        case INPUT_ACTION_ID_ROTATE_CCW:
            log_event("ROTATE CCW", event, device);
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
