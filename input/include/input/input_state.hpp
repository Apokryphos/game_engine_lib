#pragma once

#include "input/input_types.hpp"
#include <map>

namespace input
{
class InputDigitalState
{
    std::map<Input, bool> m_last;
    std::map<Input, bool> m_state;

    bool get_value(const std::map<Input, bool>& values, const Input input) const {
        if (values.find(input) != values.end()) {
            return values.at(input);
        } else {
            return bool();
        }
    }

    bool get_last_value(const Input input) const {
        return get_value(m_last, input);
    }

    bool get_value(const Input input) const {
        return get_value(m_state, input);
    }

public:
    inline bool is_down(const Input input) const {
        return get_value(input);
    }

    inline bool is_pressed(const Input input) const {
        return get_value(input) && !get_last_value(input);
    }

    inline bool is_released(const Input input) const {
        return !get_value(input) && get_last_value(input);
    }

    inline bool is_up(const Input input) const {
        return !get_value(input);
    }

    inline void press(const Input input) {
        m_state[input] = true;
    }

    inline void next() {
        m_last = m_state;
    }

    inline void release(const Input input) {
        m_state[input] = false;
    }
};

typedef InputDigitalState ButtonState;
typedef InputDigitalState KeyState;
}
