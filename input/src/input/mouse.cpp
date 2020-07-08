#include "input/input_action.hpp"
#include "input/input_action_set.hpp"
#include "input/mouse.hpp"

namespace input
{
//  ----------------------------------------------------------------------------
Mouse::Mouse(InputManager& input_mgr)
: InputDevice(input_mgr, MOUSE_DEVICE_ID),
  m_position(0) {
}

//  ----------------------------------------------------------------------------
void Mouse::button_callback(int button, int action, int mods) {
    switch (action) {
        case GLFW_PRESS:
        case GLFW_REPEAT:
            m_state.press(button);
            break;

        case GLFW_RELEASE:
            m_state.release(button);
            break;
    }
}

//  ----------------------------------------------------------------------------
void Mouse::end_poll() {
    post_events();
}

//  ----------------------------------------------------------------------------
bool Mouse::is_pressed(InputActionId id) const {
    const InputBindMap& bind_map = get_map();

    std::vector<InputBind> binds;
    bind_map.get_binds(id, InputSource::MouseButton, binds);

    for (InputBind& bind : binds) {
        if (is_button_pressed(bind.get_input())) {
            return true;
        }
    }

    return false;
}

//  ----------------------------------------------------------------------------
void Mouse::position_callback(double x, double y) {
    m_position = glm::vec2(x, y);
}

//  ----------------------------------------------------------------------------
void Mouse::post_events() {
    const InputBindMap& bind_map = get_map();

    std::vector<InputBind> binds;
    bind_map.get_binds(InputSource::MouseButton, binds);

    const InputActionSet& action_set = get_action_set();

    for (InputBind& bind : binds) {
        const InputActionId action_id = bind.get_action_id();
        const InputAction& action = action_set.get_action(action_id);
        const InputEventType event_type = action.get_event_type();
        const Button button = bind.get_input();

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
}

//  ----------------------------------------------------------------------------
void Mouse::scroll_callback(double x_offset, double y_offset) {
    m_scroll_delta = glm::vec2(x_offset, y_offset);
}

//  ----------------------------------------------------------------------------
void Mouse::start_poll() {
    m_state.next();
    m_scroll_delta = glm::vec2(0);
}
}
