#include "common/log.hpp"
#include "demo/input/input_action_ids.hpp"
#include "demo/systems/demo_system.hpp"
#include "demo/ui/demo_state.hpp"
#include "engine/base_systems/base_system_util.hpp"
#include "engine/base_systems/debug_gui_system.hpp"
#include "engine/base_systems/editor_system.hpp"
#include "engine/base_systems/name_system.hpp"
#include "engine/engine.hpp"
#include "engine/game.hpp"
#include "engine/system_manager.hpp"
#include "engine/time.hpp"
#include "input/input_device.hpp"
#include "input/input_event.hpp"
#include "input/input_manager.hpp"
#include "systems/camera_system.hpp"
#include "systems/move_system.hpp"
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

    log_event(event, device);

    SystemManager& sys_mgr = game.get_system_manager();
    DemoSystem& demo_sys = sys_mgr.get_system<DemoSystem>(SYSTEM_ID_DEMO);

    //  Check if there's an active camera
    CameraSystem& camera_sys = get_camera_system(sys_mgr);
    if (!camera_sys.has_active_camera()) {
        return;
    }

    const Entity camera = camera_sys.get_active_camera();
    const auto camera_cmpnt = camera_sys.get_component(camera);

    MoveSystem& move_sys = get_move_system(sys_mgr);
    const auto move_cmpnt = move_sys.get_component(camera);

    // const float zoom_amount =
    //     (event.get_source() == InputSource::MouseWheel) ?
    //     event.get_analog_value() :
    //     event.get_analog_value();

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

        case INPUT_ACTION_ID_MOVE_BACKWARD: {
            float amount = std::abs(event.get_analog_value());

            //  Flip sign if camera is orthographic
            const Entity camera = camera_sys.get_active_camera();
            const auto camera_cmpnt = camera_sys.get_component(camera);
            if (camera_sys.is_ortho(camera_cmpnt)) {
                amount *= -1;
            }

            move_sys.move_backward(move_cmpnt, amount);
            break;
        }

        case INPUT_ACTION_ID_MOVE_FORWARD: {
            float amount = std::abs(event.get_analog_value());

            //  Flip sign if camera is orthographic
            const Entity camera = camera_sys.get_active_camera();
            const auto camera_cmpnt = camera_sys.get_component(camera);
            if (camera_sys.is_ortho(camera_cmpnt)) {
                amount *= -1;
            }

            move_sys.move_forward(move_cmpnt, amount);
            break;
        }

        case INPUT_ACTION_ID_MOVE_RIGHT:
            move_sys.move_right(move_cmpnt, std::abs(event.get_analog_value()));
            break;

        case INPUT_ACTION_ID_MOVE_LEFT:
            move_sys.move_left(move_cmpnt, std::abs(event.get_analog_value()));
            break;

        case INPUT_ACTION_ID_AIM_HORZ:
            break;

        case INPUT_ACTION_ID_AIM_VERT:
            break;

        case INPUT_ACTION_ID_FIRE:
            break;

        case INPUT_ACTION_ID_ROTATE_CW:
            move_sys.turn_right(move_cmpnt, event.get_analog_value());
            break;

        case INPUT_ACTION_ID_ROTATE_CCW:
            move_sys.turn_left(move_cmpnt, event.get_analog_value());
            break;

        case INPUT_ACTION_ID_ZOOM_IN:
            camera_sys.zoom_in(camera_cmpnt, event.get_analog_value());
            break;

        case INPUT_ACTION_ID_ZOOM_OUT:
            camera_sys.zoom_out(camera_cmpnt, event.get_analog_value());
            break;

        case INPUT_ACTION_ID_TOGGLE_CAMERA:
            const Entity active_camera = camera_sys.get_active_camera();
            NameSystem& name_sys = get_name_system(sys_mgr);
            const auto active_camera_name_cmpnt = name_sys.get_component(active_camera);
            const std::string& active_camera_name = name_sys.get_name(active_camera_name_cmpnt);
            if (active_camera_name == "camera") {
                const Entity camera = name_sys.get_entity("ortho_camera");
                const auto camera_cmpnt = camera_sys.get_component(camera);
                camera_sys.activate(camera_cmpnt);
            } else {
                const Entity camera = name_sys.get_entity("camera");
                const auto camera_cmpnt = camera_sys.get_component(camera);
                camera_sys.activate(camera_cmpnt);
            }
            break;
    }
}
}
