#pragma once

#include "input/input_types.hpp"
#include "platform/glfw.hpp"
#include <algorithm>

namespace input
{
//  Axis analog values range from 0.0 to 1.0
//  Trigger values are scaled to this range (raw values are -1.0 to 1.0)
enum class AxisSign
{
    //  Full range of axis
    None,
    //  Negative axis values only
    Negative,
    //  Positive axis values only
    Positive,
};

static bool axis_is_trigger(const Axis axis) {
    return (
        axis == GLFW_GAMEPAD_AXIS_LEFT_TRIGGER ||
        axis == GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER
    );
}

static float axis_deadzone(float value, AxisSign sign, float deadzone) {
    switch (sign) {
        default:
        case AxisSign::None:
            return std::abs(value) > deadzone ? value : 0;

        case AxisSign::Negative:
            return value < -deadzone ? value : 0;
            break;

        case AxisSign::Positive:
            return value > deadzone ? value : 0;
            break;
    }
}

static int axis_to_digital(float value, AxisSign sign, float deadzone) {
    //  Digital is on or off (0 or 1, not -1)
    //  Axis analog values are from 0.0 to 1.0
    switch (sign) {
        default:
        case AxisSign::None:
            return std::abs(value) > deadzone ? 1 : 0;

        case AxisSign::Negative:
            return value < -deadzone ? 1 : 0;

        case AxisSign::Positive:
            return value > deadzone ? 1 : 0;
    }
}

static float get_axis_max(AxisSign sign) {
    switch (sign) {
        default:
        case AxisSign::None:
        case AxisSign::Positive:
            return 1.0f;

        case AxisSign::Negative:
            return -1.0f;
    }
}
}
