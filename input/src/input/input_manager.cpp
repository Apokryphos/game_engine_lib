#include "platform/glfw.hpp"
#include "input/gamepad.hpp"
#include "input/input_manager.hpp"
#include <cassert>
#include <stdexcept>

namespace input
{
//  To avoid using GLFW user pointer to fetch input manager
static InputManager* s_input_mgr = nullptr;

static const int GAMEPAD_COUNT = GLFW_JOYSTICK_LAST + 1;

//  ----------------------------------------------------------------------------
void glfw_joy_callback(int jid, int event) {
    s_input_mgr->joy_callback(jid, event);
}

//  ----------------------------------------------------------------------------
void glfw_key_callback(
    GLFWwindow* window,
    int key,
    int scancode,
    int action,
    int mods
) {
    s_input_mgr->key_callback(key, scancode, action, mods);
}

//  ----------------------------------------------------------------------------
InputManager::InputManager(GLFWwindow* window)
: m_keyboard(*this) {
    assert(s_input_mgr == nullptr);
    s_input_mgr = this;

    assert(window != nullptr);
    glfwSetKeyCallback(window, glfw_key_callback);
    glfwSetJoystickCallback(glfw_joy_callback);

    m_gamepads.reserve(GAMEPAD_COUNT);
    for (int n = 0; n < GAMEPAD_COUNT; ++n) {
        auto gamepad = std::make_unique<Gamepad>(*this, n);
        m_gamepads.push_back(std::move(gamepad));
    }
}

//  ----------------------------------------------------------------------------
InputManager::~InputManager() {
}

//  ----------------------------------------------------------------------------
bool InputManager::consume_event(InputEvent& event) {
    if (m_events.size() == 0) {
        return 0;
    }

    event = m_events.front();
    m_events.pop();

    return true;
}

//  ----------------------------------------------------------------------------
InputActionSet& InputManager::get_action_set() {
    return m_action_set;
}

//  ----------------------------------------------------------------------------
const InputActionSet& InputManager::get_action_set() const {
    return m_action_set;
}

//  ----------------------------------------------------------------------------
Gamepad& InputManager::get_gamepad(InputDeviceId id) {
    assert(id >= GLFW_JOYSTICK_1 && id <= GLFW_JOYSTICK_LAST);
    return *m_gamepads.at(id);
}
//  ----------------------------------------------------------------------------
InputDevice& InputManager::get_device(InputDeviceId id) {
    switch (id) {
        default:
            throw std::runtime_error("Device not found.");

        case GLFW_JOYSTICK_1:
        case GLFW_JOYSTICK_2:
        case GLFW_JOYSTICK_3:
        case GLFW_JOYSTICK_4:
        case GLFW_JOYSTICK_5:
        case GLFW_JOYSTICK_6:
        case GLFW_JOYSTICK_7:
        case GLFW_JOYSTICK_8:
        case GLFW_JOYSTICK_9:
        case GLFW_JOYSTICK_10:
        case GLFW_JOYSTICK_11:
        case GLFW_JOYSTICK_12:
        case GLFW_JOYSTICK_13:
        case GLFW_JOYSTICK_14:
        case GLFW_JOYSTICK_15:
        case GLFW_JOYSTICK_16:
            return *m_gamepads.at(id);

        case KEYBOARD_DEVICE_ID:
            return m_keyboard;
    }
}

//  ----------------------------------------------------------------------------
Keyboard& InputManager::get_keyboard() {
    return m_keyboard;
}

//  ----------------------------------------------------------------------------
void InputManager::joy_callback(int jid, int event) {
    assert(jid >= 0 && jid < m_gamepads.size());
    Gamepad* gamepad = m_gamepads.at(jid).get();

    if (event == GLFW_CONNECTED) {
        gamepad->connect();
    } else if (event == GLFW_DISCONNECTED) {
        gamepad->disconnect();
    }
}

//  ----------------------------------------------------------------------------
void InputManager::key_callback(int key, int scancode, int action, int mods) {
    m_keyboard.key_callback(key, scancode, action, mods);
}

//  ----------------------------------------------------------------------------
void InputManager::poll() {
    m_keyboard.start_poll();

    glfwPollEvents();

    //  Update gamepads
    for (int n = 0; n < GAMEPAD_COUNT; ++n) {
        Gamepad& gamepad = *m_gamepads.at(n);

        gamepad.start_poll();

        GLFWgamepadstate state{};
        if (glfwGetGamepadState(n, &state)) {
            gamepad.update_state(state);
        }

        gamepad.end_poll();
    }

    m_keyboard.end_poll();
}

//  ----------------------------------------------------------------------------
void InputManager::post_event(const InputEvent& event) {
    m_events.push(event);
}
}
