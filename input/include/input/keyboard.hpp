#pragma once

#include "input/input_device.hpp"
#include "input/input_state.hpp"
#include "input/input_types.hpp"
#include <map>

namespace input
{
const InputDeviceId KEYBOARD_DEVICE_ID = GLFW_JOYSTICK_LAST + 1;

class Keyboard : public InputDevice
{
    friend class InputManager;

    KeyState m_state;

    void end_poll();
    void key_callback(int key, int scancode, int action, int mods);
    void post_events();
    void start_poll();

public:
    Keyboard(InputManager& input_mgr);

    virtual const std::string& get_name() const override {
        static std::string name = "Keyboard";
        return name;
    }

    bool is_key_down(const Key key) const {
        return m_state.is_down(key);
    }

    virtual bool is_pressed(InputActionId id) const override;

    bool is_key_pressed(const Key key) const {
        return m_state.is_pressed(key);
    }

    bool is_key_released(const Key key) const {
        return m_state.is_released(key);
    }

    bool is_key_up(const Key key) const {
        return m_state.is_up(key);
    }
};
}
