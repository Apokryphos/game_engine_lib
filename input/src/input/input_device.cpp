#include "input/input_device.hpp"
#include "input/input_manager.hpp"

namespace input
{
//  ----------------------------------------------------------------------------
InputDevice::InputDevice(InputManager& input_mgr, InputDeviceId id)
: m_id(id),
  m_input_mgr(input_mgr) {
}

//  ----------------------------------------------------------------------------
InputDevice::~InputDevice() {
}

//  ----------------------------------------------------------------------------
const InputActionSet& InputDevice::get_action_set() const {
    return m_input_mgr.get_action_set();
}

//  ----------------------------------------------------------------------------
void InputDevice::post_event(const InputEvent& event) {
    m_input_mgr.post_event(event);
}
}
