#pragma once

#include "input/input_bind_map.hpp"
#include "input/input_event.hpp"
#include "input/input_types.hpp"

namespace input
{
class InputActionSet;
class InputManager;

class InputDevice
{
    friend class InputManager;

    InputDeviceId m_id;
    InputBindMap m_bind_map;
    InputManager& m_input_mgr;

protected:
    const InputActionSet& get_action_set() const;
    void post_event(const InputEvent& event);

public:
    InputDevice(InputManager& input_mgr, InputDeviceId id);
    virtual ~InputDevice();
    InputDevice(const InputDevice&) = delete;
    InputDevice& operator=(const InputDevice&) = delete;
    virtual const std::string& get_name() const = 0;
    virtual bool is_pressed(InputActionId id) const = 0;

    InputDeviceId get_id() const {
        return m_id;
    }

    InputBindMap& get_map() {
        return m_bind_map;
    }

    const InputBindMap& get_map() const {
        return m_bind_map;
    }

    void set_map(const InputBindMap& map) {
        m_bind_map = map;
    }
};
}
