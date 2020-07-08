#pragma once

#include "input/input_device.hpp"
#include "input/input_state.hpp"
#include "input/input_types.hpp"
#include <glm/vec2.hpp>
#include <map>

namespace input
{
const InputDeviceId MOUSE_DEVICE_ID = GLFW_JOYSTICK_LAST + 2;

class Mouse : public InputDevice
{
    friend class InputManager;

    ButtonState m_state;
    glm::vec2 m_position;
    glm::vec2 m_scroll_delta;

    void end_poll();
    void button_callback(int button, int action, int mods);
    void position_callback(double x, double y);
    void scroll_callback(double x_offset, double y_offset);
    void post_events();
    void start_poll();

public:
    Mouse(InputManager& input_mgr);

    virtual const std::string& get_name() const override {
        static std::string name = "Mouse";
        return name;
    }

    glm::vec2 get_position() const {
        return m_position;
    }

    glm::vec2 get_scroll_delta() const {
        return m_scroll_delta;
    }

    bool is_button_down(const Button button) const {
        return m_state.is_down(button);
    }

    bool is_button_pressed(const Button button) const {
        return m_state.is_pressed(button);
    }

    bool is_button_released(const Button button) const {
        return m_state.is_released(button);
    }

    bool is_button_up(const Button button) const {
        return m_state.is_up(button);
    }

    virtual bool is_pressed(InputActionId id) const override;
};
}
