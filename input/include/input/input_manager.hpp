#pragma once

#include "input/input_action_set.hpp"
#include "input/input_event.hpp"
#include "input/keyboard.hpp"
#include <memory>
#include <queue>
#include <vector>

struct GLFWwindow;

namespace input
{
class Gamepad;

class InputManager
{
    friend void glfw_key_callback(
        GLFWwindow* window,
        int key,
        int scancode,
        int action,
        int mods
    );

    friend void glfw_joy_callback(int jid, int event);

    Keyboard m_keyboard;
    std::queue<InputEvent> m_events;
    std::vector<std::unique_ptr<Gamepad>> m_gamepads;
    InputActionSet m_action_set;

    void joy_callback(int jid, int event);
    void key_callback(int key, int scancode, int action, int mods);

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
    void poll();
    void post_event(const InputEvent& event);
};
}
