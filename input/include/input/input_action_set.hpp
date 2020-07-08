#pragma once

#include "input/input_action.hpp"
#include <unordered_map>

namespace input
{
class InputActionSet
{
    std::unordered_map<InputActionId, InputAction> m_actions;

public:
    void add_action(
        const InputActionId action_id,
        InputType type,
        InputEventType event_type
    ) {
        m_actions.try_emplace(action_id, action_id, type, event_type);
    }

    const InputAction& get_action(const InputActionId action_id) const {
        return m_actions.at(action_id);
    }
};
}
