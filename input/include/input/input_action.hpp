#pragma once

#include "input/input_action_id.hpp"
#include "input/input_event_type.hpp"
#include "input/input_type.hpp"

namespace input
{
class InputAction
{
    InputEventType m_event_type;
    InputActionId m_id;
    InputType m_type;

public:
    InputAction(InputActionId id, InputType type, InputEventType event_type)
    : m_event_type(event_type),
      m_id(id),
      m_type(type) {
    }

    InputEventType get_event_type() const {
        return m_event_type;
    }

    InputActionId get_id() const {
        return m_id;
    }

    InputType get_type() const {
        return m_type;
    }

    bool is_analog() const {
        return m_type == InputType::Analog;
    }

    bool is_digital() const {
        return m_type == InputType::Digital;
    }
};
}
