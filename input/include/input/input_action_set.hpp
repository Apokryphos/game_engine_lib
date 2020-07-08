#pragma once

#include "input/input_action.hpp"
#include "input/strings/input_action_strings.hpp"
#include <string>
#include <unordered_map>

namespace input
{
class InputActionSet
{
    std::unordered_map<InputActionId, InputAction> m_actions;

public:
    void add_action(
        const InputActionId action_id,
        const std::string& name,
        InputType type,
        InputEventType event_type
    ) {
        m_actions.try_emplace(action_id, action_id, type, event_type);
        add_input_action_string(action_id, name);
    }

    const InputAction& get_action(const InputActionId action_id) const {
        return m_actions.at(action_id);
    }
};
}
