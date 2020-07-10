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
#include "engine/time.hpp"
#include "input/input_device.hpp"
#include "input/input_event.hpp"
#include "input/input_manager.hpp"
#include "systems/camera_system.hpp"
#include "systems/position_system.hpp"
#include "systems/system_util.hpp"
#include <glm/glm.hpp>

using namespace common;
using namespace ecs;
using namespace engine;
using namespace input;
using namespace systems;

namespace demo
{
static const bool LOG_EVENTS = false;

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
    const float elapsed_seconds = get_elapsed_seconds();

    Engine& engine = game.get_engine();
    InputManager& input_mgr = engine.get_input_manager();
    InputDevice& device = input_mgr.get_device(event.get_device_id());

    const InputActionId action_id = event.get_action_id();

    SystemManager& sys_mgr = game.get_system_manager();
    DemoSystem& demo_sys = sys_mgr.get_system<DemoSystem>(SYSTEM_ID_DEMO);

    //  Get active camera
    CameraSystem& camera_sys = get_camera_system(sys_mgr);
    if (!camera_sys.has_active_camera()) {
        return;
    }


    const Entity camera = camera_sys.get_active_camera();
    const auto camera_cmpnt = camera_sys.get_component(camera);

    PositionSystem& pos_sys = get_position_system(sys_mgr);
    const auto pos_cmpnt = pos_sys.get_component(camera);
    glm::vec3 camera_pos = pos_sys.get_position(pos_cmpnt);

    log_event(event, device);

    const float move_speed = 10.0f;

    const float rotate_speed = 90.0f;

    const float zoom_speed =
        (event.get_source() == InputSource::MouseWheel) ?
        5000.0f :
        100.0f;

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
            camera_pos.y += elapsed_seconds * move_speed * event.get_analog_value();
            // demo_sys.forward(elapsed_seconds * move_speed * event.get_analog_value());
            break;

        case INPUT_ACTION_ID_MOVE_UP:
            camera_pos.y -= elapsed_seconds * move_speed * event.get_analog_value();
            // demo_sys.forward(elapsed_seconds * -move_speed * event.get_analog_value());
            break;

        case INPUT_ACTION_ID_MOVE_RIGHT:
            camera_pos.x += elapsed_seconds * move_speed * event.get_analog_value();
            // demo_sys.strafe(elapsed_seconds * move_speed * event.get_analog_value());
            break;

        case INPUT_ACTION_ID_MOVE_LEFT:
            camera_pos.x -= elapsed_seconds * move_speed * event.get_analog_value();
            // demo_sys.strafe(elapsed_seconds *-move_speed * event.get_analog_value());
            break;

        case INPUT_ACTION_ID_AIM_HORZ:
            break;

        case INPUT_ACTION_ID_AIM_VERT:
            break;

        case INPUT_ACTION_ID_FIRE:
            break;

        case INPUT_ACTION_ID_ROTATE_CW:
            camera_sys.turn(
                camera_cmpnt,
                elapsed_seconds * -glm::radians(rotate_speed) * event.get_analog_value()
            );
            // demo_sys.rotate(elapsed_seconds * -glm::radians(rotate_speed) * event.get_analog_value());
            break;

        case INPUT_ACTION_ID_ROTATE_CCW:
            camera_sys.turn(
                camera_cmpnt,
                elapsed_seconds * glm::radians(rotate_speed) * event.get_analog_value()
            );
            // demo_sys.rotate(elapsed_seconds * glm::radians(rotate_speed) * event.get_analog_value());
            break;

        case INPUT_ACTION_ID_ZOOM_IN:
            demo_sys.zoom(elapsed_seconds * -zoom_speed * event.get_analog_value());
            break;

        case INPUT_ACTION_ID_ZOOM_OUT:
            demo_sys.zoom(elapsed_seconds * zoom_speed * event.get_analog_value());
            break;
    }

    pos_sys.set_position(pos_cmpnt, camera_pos);
}
}
