#pragma once

#include "input/input_device.hpp"
#include "input/input_state.hpp"
#include "input/input_types.hpp"
#include <map>

namespace input
{
const InputDeviceId MOUSE_DEVICE_ID = GLFW_JOYSTICK_LAST + 2;

class Mouse : public InputDevice
{
    friend class InputManager;

    ButtonState m_state;

    void end_poll();
    void mouse_callback(int key, int scancode, int action, int mods);
    void post_events();
    void start_poll();

public:
    Mouse(InputManager& input_mgr);

    virtual const std::string& get_name() const override {
        static std::string name = "Mouse";
        return name;
    }

    bool is_button_down(const Key key) const {
        return m_state.is_down(key);
    }

    virtual bool is_pressed(InputActionId id) const override;

    bool is_button_pressed(const Key key) const {
        return m_state.is_pressed(key);
    }

    bool is_button_released(const Key key) const {
        return m_state.is_released(key);
    }

    bool is_button_up(const Key key) const {
        return m_state.is_up(key);
    }
};
}
