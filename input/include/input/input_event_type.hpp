#pragma once

#include <string>

namespace input
{
enum class InputEventType
{
    None,
    //  Event is posted when input is in down state
    Down,
    //  Event is posted when input is pressed
    Pressed,
    //  Event is posted when input is released
    Released,
    //  Event is posted when input is in up state
    Up,
    //  Event is posted when input value changes
    //  Note that the event itself will have the type Pressed or Released.
    Delta,
    //  Event is posted every poll with current input value
    Poll,
};

inline std::string to_string(const InputEventType state) {
    switch (state) {
        default:
            return "?";
        case InputEventType::Down:
            return "Down";
        case InputEventType::Pressed:
            return "Pressed";
        case InputEventType::Up:
            return "Up";
        case InputEventType::Released:
            return "Released";
        case InputEventType::Delta:
            return "Delta";
        case InputEventType::Poll:
            return "Poll";
    }
}
}
