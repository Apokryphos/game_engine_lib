#pragma once

#include "input/gamepad_state.hpp"
#include "input/input_device.hpp"
#include "input/input_types.hpp"
#include <cstdint>
#include <string>

struct GLFWgamepadstate;

namespace input
{
class Gamepad : public InputDevice
{
    friend class InputManager;

    GamepadState m_state;
    std::string m_name;

    void connect();
    void disconnect();
    void end_poll();
    void post_events();
    void start_poll();
    void update_state(const GLFWgamepadstate& state);

public:
    Gamepad(InputManager& input_mgr, InputDeviceId id);

    virtual const std::string& get_name() const override {
        return m_name;
    }

    bool is_button_down(const Button button) const {
        return m_state.is_button_down(button);
    }

    bool is_button_pressed(const Button button) const {
        return m_state.is_button_pressed(button);
    }

    bool is_button_released(const Button button) const {
        return m_state.is_button_released(button);
    }

    bool is_button_up(const Button button) const {
        return m_state.is_button_up(button);
    }

    virtual bool is_pressed(InputActionId id) const override;
};
}
