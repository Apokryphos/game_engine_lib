#include "common/log.hpp"
#include "platform/glfw.hpp"
#include "input/input_action.hpp"
#include "input/input_action_set.hpp"
#include "input/gamepad.hpp"

using namespace common;

namespace input
{
const static float AXIS_DEADZONE = 0.3f;
const char* GAMEPAD_DISCONNECTED_NAME = "DISCONNECTED";

//  ----------------------------------------------------------------------------
static const char* get_gamepad_name(InputDeviceId id) {
    const char* name = glfwGetGamepadName(id);
    return name == NULL ? GAMEPAD_DISCONNECTED_NAME : name;
}

//  ----------------------------------------------------------------------------
Gamepad::Gamepad(InputManager& input_mgr, InputDeviceId id)
: InputDevice(input_mgr, id),
    m_name(get_gamepad_name(id)) {
}

//  ----------------------------------------------------------------------------
void Gamepad::connect() {
    const InputDeviceId id = get_id();
    m_name = get_gamepad_name(id);
    log_debug("Gamepad '%s' (%d) connected.", m_name.c_str(), id);
}

//  ----------------------------------------------------------------------------
void Gamepad::disconnect() {
    const InputDeviceId id = get_id();
    log_debug("Gamepad '%s' (%d) disconnected.", m_name.c_str(), id);
    m_name = GAMEPAD_DISCONNECTED_NAME;

    //  Prevent stuck events on disconnect
    m_state.reset();
    post_events();
}

//  ----------------------------------------------------------------------------
void Gamepad::end_poll() {
    post_events();
}

//  ----------------------------------------------------------------------------
bool Gamepad::is_pressed(InputActionId id) const {
    const InputBindMap& bind_map = get_map();

    std::vector<InputBind> binds;
    bind_map.get_binds(id, InputSource::Key, binds);

    for (InputBind& bind : binds) {
        if (is_button_pressed(bind.get_input())) {
            return true;
        }
    }

    return false;
}

//  ----------------------------------------------------------------------------
void Gamepad::post_events() {
    if (!glfwJoystickIsGamepad(get_id())) {
        return;
    }

    const InputBindMap& bind_map = get_map();

    const InputActionSet& action_set = get_action_set();

    const std::vector<InputBind>& binds = bind_map.get_binds();

    for (const InputBind& bind : binds) {
        const InputActionId action_id = bind.get_action_id();
        const InputSource source = bind.get_source();

        switch (source) {
           case InputSource::Axis: {
                const Axis axis = bind.get_input();
                const AxisSign sign = bind.get_axis_sign();

                const InputAction& action = action_set.get_action(action_id);
                const InputEventType event_type = action.get_event_type();
                const InputType type = action.get_type();

                switch (event_type) {
                    case InputEventType::Down: {
                        if (m_state.is_axis_down(axis, sign, type, AXIS_DEADZONE)) {
                            const InputType type = action.get_type();

                            const InputEvent event(
                                get_id(),
                                action_id,
                                InputEventType::Down,
                                m_state.get_axis_value(axis, sign, type, AXIS_DEADZONE)
                            );

                            InputDevice::post_event(event);
                        }
                    }
                    break;

                    case InputEventType::Pressed: {
                        if (m_state.is_axis_pressed(axis, sign, type, AXIS_DEADZONE)) {
                            const InputEvent event(
                                get_id(),
                                action_id,
                                InputEventType::Pressed,
                                get_axis_max(sign)
                            );
                            InputDevice::post_event(event);
                        }
                    }
                    break;

                    case InputEventType::Released: {
                        if (m_state.is_axis_released(axis, sign, type, AXIS_DEADZONE)) {
                            const InputEvent event(
                                get_id(),
                                action_id,
                                InputEventType::Released,
                                0
                            );
                            InputDevice::post_event(event);
                        }
                    }
                    break;

                    case InputEventType::Up: {
                        if (m_state.is_axis_up(axis, sign, type, AXIS_DEADZONE)) {
                            const InputEvent event(
                                get_id(),
                                action_id,
                                InputEventType::Up,
                                0
                            );
                            InputDevice::post_event(event);
                        }
                    }
                    break;

                    case InputEventType::Delta: {
                        if (m_state.axis_changed(axis, sign, type, AXIS_DEADZONE)) {
                            const float value = m_state.get_axis_value(
                                axis,
                                sign,
                                type,
                                AXIS_DEADZONE
                            );

                            const InputEvent event(
                                get_id(),
                                action_id,
                                value == 0 ? InputEventType::Released : InputEventType::Pressed,
                                value
                            );

                            InputDevice::post_event(event);
                        }
                    }
                    break;

                    case InputEventType::Poll: {
                        const InputType type = action.get_type();

                        const float value = m_state.get_axis_value(
                            axis,
                            sign,
                            type,
                            AXIS_DEADZONE
                        );
                        const InputEvent event(
                            get_id(),
                            action_id,
                            InputEventType::Poll,
                            value
                        );
                        InputDevice::post_event(event);
                    }
                    break;
                }
            }
            break;

            case InputSource::Button: {
                const InputAction& action = action_set.get_action(action_id);
                const InputEventType event_type = action.get_event_type();
                const Key button = bind.get_input();

                switch (event_type) {
                    case InputEventType::Down: {
                        if (is_button_down(button)) {
                            InputEvent event(get_id(), action_id, InputEventType::Down, 1);
                            InputDevice::post_event(event);
                        }
                    }
                    break;

                    case InputEventType::Pressed: {
                        if (is_button_pressed(button)) {
                            InputEvent event(get_id(), action_id, InputEventType::Pressed, 1);
                            InputDevice::post_event(event);
                        }
                    }
                    break;

                    case InputEventType::Released: {
                        if (is_button_released(button)) {
                            InputEvent event(get_id(), action_id, InputEventType::Released, 0);
                            InputDevice::post_event(event);
                        }
                    }
                    break;

                    case InputEventType::Up: {
                        if (is_button_up(button)) {
                            InputEvent event(get_id(), action_id, InputEventType::Up, 0);
                            InputDevice::post_event(event);
                        }
                    }
                    break;

                    case InputEventType::Delta: {
                        int value = 0;
                        InputEventType event_type;

                        if (is_button_pressed(button)) {
                            value = 1;
                            event_type = InputEventType::Pressed;
                        } else if (is_button_released(button)) {
                            value = -1;
                            event_type = InputEventType::Released;
                        }

                        if (value != 0) {
                            InputEvent event(get_id(), action_id, event_type, value);
                            InputDevice::post_event(event);
                        }
                    }
                    break;

                    case InputEventType::Poll: {
                        const int value = is_button_down(button) ? 1 : 0;
                        InputEvent event(get_id(), action_id, InputEventType::Poll, value);
                        InputDevice::post_event(event);
                    }
                    break;
                }
            }
            break;
        }
    }
}

//  ----------------------------------------------------------------------------
void Gamepad::start_poll() {
    m_state.new_frame();
}

//  ----------------------------------------------------------------------------
void Gamepad::update_state(const GLFWgamepadstate& state) {
    m_state.update(state);
}
}
