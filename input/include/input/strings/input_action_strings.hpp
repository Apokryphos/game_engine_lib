#pragma once

#include "common/language.hpp"
#include "input/input_action_id.hpp"
#include <string>
#include <vector>

namespace input
{
struct InputActionStringEntry
{
    InputActionId action_id;
    std::string name;

    //  Name used in config files
    std::string config_name;
};

void add_input_action_string(InputActionId action_id, const std::string& name);
InputActionId get_input_action_by_name(const std::string& name);
const InputActionStringEntry* get_input_action_string(const InputActionId action_id);
const std::vector<InputActionStringEntry>& get_input_action_strings();
}
