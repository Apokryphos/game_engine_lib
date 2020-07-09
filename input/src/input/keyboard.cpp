#include "platform/glfw.hpp"
#include "input/input_action.hpp"
#include "input/input_action_set.hpp"
#include "input/keyboard.hpp"

namespace input
{
//  ----------------------------------------------------------------------------
Keyboard::Keyboard(InputManager& input_mgr)
: InputDevice(input_mgr, KEYBOARD_DEVICE_ID) {
}

//  ----------------------------------------------------------------------------
void Keyboard::end_poll() {
    post_events();
}

//  ----------------------------------------------------------------------------
bool Keyboard::is_pressed(InputActionId id) const {
    const InputBindMap& bind_map = get_map();

    std::vector<InputBind> binds;
    bind_map.get_binds(id, InputSource::Key, binds);

    for (InputBind& bind : binds) {
        if (is_key_pressed(bind.get_input())) {
            return true;
        }
    }

    return false;
}

//  ----------------------------------------------------------------------------
void Keyboard::key_callback(int key, int scancode, int action, int mods) {
    switch (action) {
        case GLFW_PRESS:
        case GLFW_REPEAT:
            m_state.press(key);
            break;

        case GLFW_RELEASE:
            m_state.release(key);
            break;
    }
}

//  ----------------------------------------------------------------------------
void Keyboard::post_events() {
    const InputBindMap& bind_map = get_map();

    std::vector<InputBind> binds;
    bind_map.get_binds(InputSource::Key, binds);

    const InputActionSet& action_set = get_action_set();

    for (InputBind& bind : binds) {
        const InputActionId action_id = bind.get_action_id();
        const InputAction& action = action_set.get_action(action_id);
        const InputEventType event_type = action.get_event_type();
        const Key key = bind.get_input();

        switch (event_type) {
            case InputEventType::Down: {
                if (is_key_down(key)) {
                    InputEvent event(get_id(), action_id, InputEventType::Down, InputSource::Key, 1);
                    InputDevice::post_event(event);
                }
            }
            break;

            case InputEventType::Pressed: {
                if (is_key_pressed(key)) {
                    InputEvent event(get_id(), action_id, InputEventType::Pressed, InputSource::Key, 1);
                    InputDevice::post_event(event);
                }
            }
            break;

            case InputEventType::Released: {
                if (is_key_released(key)) {
                    InputEvent event(get_id(), action_id, InputEventType::Released, InputSource::Key, 0);
                    InputDevice::post_event(event);
                }
            }
            break;

            case InputEventType::Up: {
                if (is_key_up(key)) {
                    InputEvent event(get_id(), action_id, InputEventType::Up, InputSource::Key, 0);
                    InputDevice::post_event(event);
                }
            }
            break;

            case InputEventType::Delta: {
                int value = 0;
                InputEventType event_type;

                if (is_key_pressed(key)) {
                    value = 1;
                    event_type = InputEventType::Pressed;
                } else if (is_key_released(key)) {
                    value = -1;
                    event_type = InputEventType::Released;
                }

                if (value != 0) {
                    InputEvent event(get_id(), action_id, event_type, InputSource::Key, value);
                    InputDevice::post_event(event);
                }
            }
            break;

            case InputEventType::Poll: {
                const int value = is_key_down(key) ? 1 : 0;
                InputEvent event(get_id(), action_id, InputEventType::Poll, InputSource::Key, value);
                InputDevice::post_event(event);
            }
            break;
        }
    }
}

//  ----------------------------------------------------------------------------
void Keyboard::start_poll() {
    m_state.next();
}
}
