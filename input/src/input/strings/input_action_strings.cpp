#include "common/string.hpp"
#include "input/strings/input_action_strings.hpp"
#include <cassert>

using namespace common;

namespace input
{
static std::vector<InputActionStringEntry> s_entries;

//  ----------------------------------------------------------------------------
inline static InputActionStringEntry make_entry(
    InputActionId action_id,
    const std::string& name
) {
    std::string config_name = name;
    to_lowercase(config_name);
    replace(config_name, ' ', '_');
    return { action_id, name ,config_name };
}

//  ----------------------------------------------------------------------------
void add_input_action_string(InputActionId action_id, const std::string& name) {
    const auto find = std::find_if(
        s_entries.begin(),
        s_entries.end(),
        [action_id, name](const auto& entry) {
            return (
                entry.action_id == action_id ||
                entry.name == name
            );
        }
    );

    if (find != s_entries.end()) {
        throw std::runtime_error("An input action must have a unique ID and name.");
    }

    InputActionStringEntry entry = make_entry(action_id, name);
    s_entries.push_back(entry);
}

//  ----------------------------------------------------------------------------
InputActionId get_input_action_by_name(const std::string& name) {
    for (const auto& entry : s_entries) {
        if (entry.name == name) {
            return entry.action_id;
        }
    }

    return INPUT_ACTION_ID_UNASSIGNED;
}

//  ----------------------------------------------------------------------------
const InputActionStringEntry* get_input_action_string(
    const InputActionId action_id
) {
    for (const auto& entry : s_entries) {
        if (entry.action_id == action_id) {
            return &entry;
        }
    }

    return nullptr;
}

//  ----------------------------------------------------------------------------
const std::vector<InputActionStringEntry>& get_input_action_strings() {
    return s_entries;
}
}
