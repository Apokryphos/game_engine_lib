#pragma once

#include "input/input_action_id.hpp"
#include "input/input_event_type.hpp"
#include "input/input_source.hpp"
#include "input/input_types.hpp"

namespace input
{
class InputEvent
{
    InputDeviceId m_device_id;
    InputActionId m_action_id;
    InputEventType m_event_type;
    InputSource m_source;
    float m_value;

public:
    InputEvent(
        InputDeviceId device_id = 0,
        InputActionId action_id = INPUT_ACTION_ID_UNASSIGNED,
        InputEventType event_type = InputEventType::None,
        InputSource source = InputSource::None,
        float value = 0
    )
    : m_device_id(device_id),
      m_action_id(action_id),
      m_event_type(event_type),
      m_source(source),
      m_value(value) {
    }

    InputActionId get_action_id() const {
        return m_action_id;
    }

    float get_analog_value() const {
        return m_value;
    }

    InputDeviceId get_device_id() const {
        return m_device_id;
    }

    int get_digital_value() const {
        //  Digital is on or off (0 or 1...not -1)
        return (m_value != 0) ? 1 : 0;
    }

    InputSource get_source() const {
        return m_source;
    }

    InputEventType get_type() const {
        return m_event_type;
    }
};
}
