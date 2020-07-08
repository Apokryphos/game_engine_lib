#include "common/string.hpp"
#include "input/strings/input_strings.hpp"
#include <vector>

using namespace common;

namespace input
{
static std::vector<InputStringEntry> s_entries;

//  ----------------------------------------------------------------------------
static InputStringEntry make_button(
    const int input,
    const std::string& name
) {
    InputStringEntry entry;
    entry.name = name;
    entry.input = input;
    entry.source = InputSource::Button;
    entry.axis_sign = AxisSign::None;

    std::string config_name = "gamepad." + name;
    to_lowercase(config_name);
    replace(config_name, ' ', '_');
    entry.config_name = config_name;

    return entry;
}

//  ----------------------------------------------------------------------------
static InputStringEntry make_axis(
    const int input,
    const std::string& name,
    const AxisSign axis_sign
) {
    InputStringEntry entry;
    entry.name = name + axis_sign_to_symbol(axis_sign);
    entry.input = input;
    entry.source = InputSource::Axis;
    entry.axis_sign = axis_sign;

    std::string config_name = "gamepad." + name;
    replace(config_name, ' ', '_');
    config_name += "." + axis_sign_to_string(axis_sign);
    to_lowercase(config_name);

    entry.config_name = config_name;

    return entry;
}

//  ----------------------------------------------------------------------------
static InputStringEntry make_key(
    const int input,
    const std::string& name
) {
    InputStringEntry entry;
    entry.name = name;
    entry.input = input;
    entry.source = InputSource::Key;
    entry.axis_sign = AxisSign::None;

    std::string config_name = "key." + name;
    to_lowercase(config_name);
    replace(config_name, ' ', '_');
    entry.config_name = config_name;

    return entry;
}

//  ----------------------------------------------------------------------------
static InputStringEntry make_mouse(
    const int input,
    const std::string& name
) {
    InputStringEntry entry;
    entry.name = name;
    entry.input = input;
    entry.source = InputSource::MouseButton;
    entry.axis_sign = AxisSign::None;

    std::string config_name = name;
    to_lowercase(config_name);
    replace(config_name, ' ', '.');
    entry.config_name = config_name;

    return entry;
}

//  ----------------------------------------------------------------------------
const InputStringEntry* get_input_string(const std::string& config_name) {
    for (const auto& entry : s_entries) {
        if (entry.config_name == config_name) {
            return &entry;
        }
    }

    return nullptr;
}

//  ----------------------------------------------------------------------------
const std::vector<InputStringEntry>& get_input_strings() {
    return s_entries;
}

//  ----------------------------------------------------------------------------
void initialize_input_strings() {
    s_entries = {
        //  Mouse buttons
        make_mouse(GLFW_MOUSE_BUTTON_LEFT, "Mouse Left"),
        make_mouse(GLFW_MOUSE_BUTTON_RIGHT, "Mouse Right"),
        make_mouse(GLFW_MOUSE_BUTTON_MIDDLE, "Mouse Middle"),
        make_mouse(GLFW_MOUSE_BUTTON_4, "Mouse 4"),
        make_mouse(GLFW_MOUSE_BUTTON_5, "Mouse 5"),
        make_mouse(GLFW_MOUSE_BUTTON_6, "Mouse 6"),
        make_mouse(GLFW_MOUSE_BUTTON_7, "Mouse 7"),
        make_mouse(GLFW_MOUSE_BUTTON_8, "Mouse 8"),

        //  Gamepad buttons
        make_button(GLFW_GAMEPAD_BUTTON_A, "A"),
        make_button(GLFW_GAMEPAD_BUTTON_B, "B"),
        make_button(GLFW_GAMEPAD_BUTTON_X, "X"),
        make_button(GLFW_GAMEPAD_BUTTON_Y, "Y"),
        make_button(GLFW_GAMEPAD_BUTTON_LEFT_BUMPER, "Left Bumper"),
        make_button(GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER, "Right Bumper"),
        make_button(GLFW_GAMEPAD_BUTTON_BACK, "Back"),
        make_button(GLFW_GAMEPAD_BUTTON_START, "Start"),
        make_button(GLFW_GAMEPAD_BUTTON_GUIDE, "Guide"),
        make_button(GLFW_GAMEPAD_BUTTON_LEFT_THUMB, "Left Thumb"),
        make_button(GLFW_GAMEPAD_BUTTON_RIGHT_THUMB, "Right Thumb"),
        make_button(GLFW_GAMEPAD_BUTTON_DPAD_UP, "DPad Up"),
        make_button(GLFW_GAMEPAD_BUTTON_DPAD_RIGHT, "DPad Right"),
        make_button(GLFW_GAMEPAD_BUTTON_DPAD_DOWN, "DPad Down"),
        make_button(GLFW_GAMEPAD_BUTTON_DPAD_LEFT, "DPad Left"),

        //  Gamepad axes
        make_axis(GLFW_GAMEPAD_AXIS_LEFT_X, "Left X", AxisSign::Positive),
        make_axis(GLFW_GAMEPAD_AXIS_LEFT_X, "Left X", AxisSign::Negative),
        make_axis(GLFW_GAMEPAD_AXIS_LEFT_Y, "Left Y", AxisSign::Positive),
        make_axis(GLFW_GAMEPAD_AXIS_LEFT_Y, "Left Y", AxisSign::Negative),
        make_axis(GLFW_GAMEPAD_AXIS_RIGHT_X, "Right X", AxisSign::Positive),
        make_axis(GLFW_GAMEPAD_AXIS_RIGHT_X, "Right X", AxisSign::Negative),
        make_axis(GLFW_GAMEPAD_AXIS_RIGHT_Y, "Right Y", AxisSign::Positive),
        make_axis(GLFW_GAMEPAD_AXIS_RIGHT_Y, "Right Y", AxisSign::Negative),
        make_axis(GLFW_GAMEPAD_AXIS_LEFT_TRIGGER, "Left Trigger", AxisSign::Positive),
        make_axis(GLFW_GAMEPAD_AXIS_LEFT_TRIGGER, "Left Trigger", AxisSign::Negative),
        make_axis(GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER, "Right Trigger", AxisSign::Positive),
        make_axis(GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER, "Right Trigger", AxisSign::Negative),

        //  Keys
        make_key(GLFW_KEY_SPACE, "Space"),
        make_key(GLFW_KEY_APOSTROPHE, "Apostrophe"),
        make_key(GLFW_KEY_COMMA, "Comma"),
        make_key(GLFW_KEY_MINUS, "Minus"),
        make_key(GLFW_KEY_PERIOD, "Period"),
        make_key(GLFW_KEY_SLASH, "Slash"),
        make_key(GLFW_KEY_0, "0"),
        make_key(GLFW_KEY_1, "1"),
        make_key(GLFW_KEY_2, "2"),
        make_key(GLFW_KEY_3, "3"),
        make_key(GLFW_KEY_4, "4"),
        make_key(GLFW_KEY_5, "5"),
        make_key(GLFW_KEY_6, "6"),
        make_key(GLFW_KEY_7, "7"),
        make_key(GLFW_KEY_8, "8"),
        make_key(GLFW_KEY_9, "9"),
        make_key(GLFW_KEY_SEMICOLON, "Semicolon"),
        make_key(GLFW_KEY_EQUAL, "Equals"),
        make_key(GLFW_KEY_A, "A"),
        make_key(GLFW_KEY_B, "B"),
        make_key(GLFW_KEY_C, "C"),
        make_key(GLFW_KEY_D, "D"),
        make_key(GLFW_KEY_E, "E"),
        make_key(GLFW_KEY_F, "F"),
        make_key(GLFW_KEY_G, "G"),
        make_key(GLFW_KEY_H, "H"),
        make_key(GLFW_KEY_I, "I"),
        make_key(GLFW_KEY_J, "J"),
        make_key(GLFW_KEY_K, "K"),
        make_key(GLFW_KEY_L, "L"),
        make_key(GLFW_KEY_M, "M"),
        make_key(GLFW_KEY_N, "N"),
        make_key(GLFW_KEY_O, "O"),
        make_key(GLFW_KEY_P, "P"),
        make_key(GLFW_KEY_Q, "Q"),
        make_key(GLFW_KEY_R, "R"),
        make_key(GLFW_KEY_S, "S"),
        make_key(GLFW_KEY_T, "T"),
        make_key(GLFW_KEY_U, "U"),
        make_key(GLFW_KEY_V, "V"),
        make_key(GLFW_KEY_W, "W"),
        make_key(GLFW_KEY_X, "X"),
        make_key(GLFW_KEY_Y, "Y"),
        make_key(GLFW_KEY_Z, "Z"),
        make_key(GLFW_KEY_LEFT_BRACKET, "Left Bracket"),
        make_key(GLFW_KEY_BACKSLASH, "Backslash"),
        make_key(GLFW_KEY_RIGHT_BRACKET, "Right Bracket"),
        make_key(GLFW_KEY_GRAVE_ACCENT, "Grave"),
        make_key(GLFW_KEY_WORLD_1, "World 1"),
        make_key(GLFW_KEY_WORLD_2, "World 2"),
        make_key(GLFW_KEY_ESCAPE, "Escape"),
        make_key(GLFW_KEY_ENTER, "Enter"),
        make_key(GLFW_KEY_TAB, "Tab"),
        make_key(GLFW_KEY_BACKSPACE, "Backspace"),
        make_key(GLFW_KEY_INSERT, "Insert"),
        make_key(GLFW_KEY_DELETE, "Delete"),
        make_key(GLFW_KEY_RIGHT, "Right Arrow"),
        make_key(GLFW_KEY_LEFT, "Left Arrow"),
        make_key(GLFW_KEY_DOWN, "Down Arrow"),
        make_key(GLFW_KEY_UP, "Up Arrow"),
        make_key(GLFW_KEY_PAGE_UP, "Page Up"),
        make_key(GLFW_KEY_PAGE_DOWN, "Page Down"),
        make_key(GLFW_KEY_HOME, "Home"),
        make_key(GLFW_KEY_END, "End"),
        make_key(GLFW_KEY_CAPS_LOCK, "Caps Lock"),
        make_key(GLFW_KEY_SCROLL_LOCK, "Scroll Lock"),
        make_key(GLFW_KEY_NUM_LOCK, "Num Lock"),
        make_key(GLFW_KEY_PRINT_SCREEN, "Print Screen"),
        make_key(GLFW_KEY_PAUSE, "Pause"),
        make_key(GLFW_KEY_F1, "F1"),
        make_key(GLFW_KEY_F2, "F2"),
        make_key(GLFW_KEY_F3, "F3"),
        make_key(GLFW_KEY_F4, "F4"),
        make_key(GLFW_KEY_F5, "F5"),
        make_key(GLFW_KEY_F6, "F6"),
        make_key(GLFW_KEY_F7, "F7"),
        make_key(GLFW_KEY_F8, "F8"),
        make_key(GLFW_KEY_F9, "F9"),
        make_key(GLFW_KEY_F10, "F10"),
        make_key(GLFW_KEY_F11, "F11"),
        make_key(GLFW_KEY_F12, "F12"),
        make_key(GLFW_KEY_F13, "F13"),
        make_key(GLFW_KEY_F14, "F14"),
        make_key(GLFW_KEY_F15, "F15"),
        make_key(GLFW_KEY_F16, "F16"),
        make_key(GLFW_KEY_F17, "F17"),
        make_key(GLFW_KEY_F18, "F18"),
        make_key(GLFW_KEY_F19, "F19"),
        make_key(GLFW_KEY_F20, "F20"),
        make_key(GLFW_KEY_F21, "F21"),
        make_key(GLFW_KEY_F22, "F22"),
        make_key(GLFW_KEY_F23, "F23"),
        make_key(GLFW_KEY_F24, "F24"),
        make_key(GLFW_KEY_F25, "F25"),
        make_key(GLFW_KEY_KP_0, "Keypad 0"),
        make_key(GLFW_KEY_KP_1, "Keypad 1"),
        make_key(GLFW_KEY_KP_2, "Keypad 2"),
        make_key(GLFW_KEY_KP_3, "Keypad 3"),
        make_key(GLFW_KEY_KP_4, "Keypad 4"),
        make_key(GLFW_KEY_KP_5, "Keypad 5"),
        make_key(GLFW_KEY_KP_6, "Keypad 6"),
        make_key(GLFW_KEY_KP_7, "Keypad 7"),
        make_key(GLFW_KEY_KP_8, "Keypad 8"),
        make_key(GLFW_KEY_KP_9, "Keypad 9"),
        make_key(GLFW_KEY_KP_DECIMAL, "Decimal"),
        make_key(GLFW_KEY_KP_DIVIDE, "Divide"),
        make_key(GLFW_KEY_KP_MULTIPLY, "Multiply"),
        make_key(GLFW_KEY_KP_SUBTRACT, "Subtract"),
        make_key(GLFW_KEY_KP_ADD, "Add"),
        make_key(GLFW_KEY_KP_ENTER, "Keypad Enter"),
        make_key(GLFW_KEY_KP_EQUAL, "Keypad Equal"),
        make_key(GLFW_KEY_LEFT_SHIFT, "Left Shift"),
        make_key(GLFW_KEY_LEFT_CONTROL, "Left Control"),
        make_key(GLFW_KEY_LEFT_ALT, "Left Alt"),
        make_key(GLFW_KEY_LEFT_SUPER, "Left Super"),
        make_key(GLFW_KEY_RIGHT_SHIFT, "Right Shift"),
        make_key(GLFW_KEY_RIGHT_CONTROL, "Right Control"),
        make_key(GLFW_KEY_RIGHT_ALT, "Right Alt"),
        make_key(GLFW_KEY_RIGHT_SUPER, "Right Super"),
        make_key(GLFW_KEY_MENU, "Menu"),
    };
}
}
