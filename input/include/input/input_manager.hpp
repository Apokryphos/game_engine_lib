#pragma once

#include "input/input_action_set.hpp"
#include "input/input_event.hpp"
#include "input/keyboard.hpp"
#include "input/mouse.hpp"
#include <memory>
#include <queue>
#include <vector>

struct GLFWwindow;

namespace input
{
class Gamepad;
class InputBindMap;

class InputManager
{
    friend void glfw_cursor_position_callback(
        GLFWwindow* window,
        double xpos,
        double ypos
    );

    friend void glfw_key_callback(
        GLFWwindow* window,
        int key,
        int scancode,
        int action,
        int mods
    );

    friend void glfw_joy_callback(int jid, int event);

    friend void glfw_mouse_button_callback(
        GLFWwindow* window,
        int button,
        int action,
        int mods
    );

    Keyboard m_keyboard;
    Mouse m_mouse;
    std::queue<InputEvent> m_events;
    std::vector<std::unique_ptr<Gamepad>> m_gamepads;
    InputActionSet m_action_set;

    void joy_callback(int jid, int event);
    void key_callback(int key, int scancode, int action, int mods);
    void mouse_button_callback(int button, int action, int mods);
    void mouse_position_callback(double xpos, double ypos);

public:
    InputManager(GLFWwindow* window);
    ~InputManager();
    InputManager(const InputManager&) = delete;
    InputManager& operator=(const InputManager&) = delete;
    bool consume_event(InputEvent& event);
    InputActionSet& get_action_set();
    const InputActionSet& get_action_set() const;
    Gamepad& get_gamepad(InputDeviceId id);
    InputDevice& get_device(InputDeviceId id);
    Keyboard& get_keyboard();
    Mouse& get_mouse();
    void poll();
    void post_event(const InputEvent& event);

    //  Sets the input bind map for every gamepad
    void set_gamepad_maps(const InputBindMap& map);
};
}
