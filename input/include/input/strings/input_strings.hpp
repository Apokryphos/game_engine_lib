#pragma once

#include "input/input_source.hpp"
#include "input/axis_sign.hpp"
#include <string>

namespace input
{
//  Contains string data for inputs (keys, buttons, etc.)
struct InputStringEntry
{
    //  Input (GLFW)
    int input;

    //  Source (Axis, Button, Key, etc.)
    InputSource source;

    //  Sign of axis (Positive, Negative, None)
    AxisSign axis_sign;

    //  String used in config files
    std::string config_name;

    //  Printable name
    std::string name;
};

//  ----------------------------------------------------------------------------
inline std::string axis_sign_to_symbol(const AxisSign sign) {
    if (sign == AxisSign::Positive) {
        return "+";
    } if (sign == AxisSign::Negative) {
        return "-";
    } else {
        return "";
    }
}

//  ----------------------------------------------------------------------------
inline std::string axis_sign_to_string(const AxisSign sign) {
    if (sign == AxisSign::Positive) {
        return "Positive";
    } if (sign == AxisSign::Negative) {
        return "Negative";
    } else {
        return "None";
    }
}

//  ----------------------------------------------------------------------------
void initialize_input_strings();
const InputStringEntry* get_input_string(const std::string& config_name);
const std::vector<InputStringEntry>& get_input_strings();
}
