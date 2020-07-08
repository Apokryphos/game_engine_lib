#pragma once

#include "input/axis_sign.hpp"
#include "input/input_type.hpp"
#include "input/input_types.hpp"
#include "platform/glfw.hpp"
#include <cassert>

namespace input
{
class GamepadState
{
    static constexpr float AXIS_MIN = 0.0f;

    //  Frame counter for preventing false inputs
    int m_frame;
    GLFWgamepadstate m_last;
    GLFWgamepadstate m_state;

    //  Returns the axis value after applying the deadzone
    static float get_axis_value(
        const GLFWgamepadstate& state,
        Axis axis,
        AxisSign sign,
        InputType type,
        float deadzone
    ) {
        float raw = state.axes[axis];

        //  Trigger values are -1.0 to 1.0
        if (axis_is_trigger(axis)) {
            assert(sign == AxisSign::None);

            //  Scale trigger value to [0.0, 1.0]
            raw = (raw + 1.0) / 2.0;
        }

        //  Apply deadzone
        raw = axis_deadzone(raw, sign, deadzone);


        if (type == InputType::Digital) {
            return axis_to_digital(raw, sign, deadzone);
        }

        return raw;
    }

    float get_last_axis_value(Axis axis, AxisSign sign, InputType type, float deadzone) const {
        return get_axis_value(m_last, axis, sign, type, deadzone);
    }

public:
    GamepadState()
    : m_last({0}),
      m_state({0}),
      m_frame(0) {
    }

    bool axis_changed(
        const Axis axis,
        const AxisSign sign,
        const InputType type,
        const float deadzone
    ) {
        const float current = get_axis_value(axis, sign, type, deadzone);
        const float last = get_last_axis_value(axis, sign, type, deadzone);
        return current != last;
    }

    float get_axis_value(
        Axis axis,
        AxisSign sign,
        InputType type,
        float deadzone
    ) const {
        return get_axis_value(m_state, axis, sign, type, deadzone);
    }

    bool is_axis_down(
        const Axis axis,
        const AxisSign sign,
        InputType type,
        float deadzone
    ) const {
        return get_axis_value(m_state, axis, sign, type, deadzone) != AXIS_MIN;
    }

    bool is_axis_pressed(
        const Axis axis,
        const AxisSign sign,
        InputType type,
        float deadzone
    ) const {
        return (
            get_axis_value(m_state, axis, sign, type, deadzone) != AXIS_MIN &&
            get_axis_value(m_last, axis, sign, type, deadzone) == AXIS_MIN
        );
    }

    bool is_axis_released(
        const Axis axis,
        const AxisSign sign,
        InputType type,
        float deadzone
    ) const {
        return (
            get_axis_value(m_state, axis, sign, type, deadzone) == AXIS_MIN &&
            get_axis_value(m_last, axis, sign, type, deadzone) != AXIS_MIN
        );
    }

    bool is_axis_up(
        const Axis axis,
        const AxisSign sign,
        InputType type,
        float deadzone
    ) const {
        return get_axis_value(m_state, axis, sign, type, deadzone) == AXIS_MIN;
    }

    bool is_button_down(const Button button) const {
        return m_state.buttons[button] == GLFW_PRESS;
    }

    bool is_button_pressed(const Button button) const {
        return m_state.buttons[button] && !m_last.buttons[button];
    }

    bool is_button_released(const Button button) const {
        return !m_state.buttons[button] && m_last.buttons[button];
    }

    bool is_button_up(const Button button) const {
        return m_state.buttons[button] == GLFW_RELEASE;
    }

    //  Updates state arrays for a new frame.
    //  Saves the current state array as last.
    //  Clears the current state array.
    void new_frame() {
        m_last = m_state;
        m_state = {0};
    }

    //  Call when gamepad is disconnected
    //  Resets state arrays and frame counter
    void reset() {
        m_frame = 0;
        m_state = {0};
        m_last = {0};
    }

    //  Updates the current state
    void update(const GLFWgamepadstate& state) {
        m_state = state;

        if (m_frame == 0) {
            //  Prevent false inputs on first update
            m_last = m_state;
            ++m_frame;
        }
    }
};
}
