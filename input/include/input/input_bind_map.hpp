#pragma once

#include "input/axis_sign.hpp"
#include "input/input_bind.hpp"
#include "input/input_types.hpp"
#include <algorithm>
#include <cassert>
#include <vector>

namespace input
{
//  A collection of bindings between input actions and sources
class InputBindMap
{
    std::vector<InputBind> m_binds;

public:
    bool axis_is_bound(const Axis axis, const AxisSign sign) const {
        const auto find = std::find_if(
            m_binds.begin(),
            m_binds.end(),
            [axis, sign](const InputBind& bind) {
                return bind.is_axis(axis, sign);
            }
        );

        return find != m_binds.end();
    }

    void bind_axis(
        const InputActionId action_id,
        const Axis axis,
        AxisSign sign = AxisSign::None
    ) {
        if (axis_is_bound(axis, sign)) {
            clear_axis(axis, sign);
        }
        assert(!axis_is_bound(axis, sign));

        //  Triggers cannot use partial axis range
        if (axis_is_trigger(axis)) {
            sign = AxisSign::None;
        }

        m_binds.push_back(InputBind::make_axis(action_id, axis, sign));
    }

    void bind_button(const InputActionId action_id, const Button button) {
        if (button_is_bound(button)) {
            clear_button(button);
        }
        assert(!button_is_bound(button));

        m_binds.push_back(InputBind::make_button(action_id, button));
    }

    void bind_key(const InputActionId action_id, const Key key) {
        if (key_is_bound(key)) {
            clear_key(key);
        }
        assert(!key_is_bound(key));

        m_binds.push_back(InputBind::make_key(action_id, key));
    }

    void bind_mouse_button(const InputActionId action_id, const Button button) {
        if (mouse_button_is_bound(button)) {
            clear_mouse_button(button);
        }
        assert(!mouse_button_is_bound(button));

        m_binds.push_back(InputBind::make_mouse_button(action_id, button));
    }

    void bind_mouse_wheel(const InputActionId action_id, const AxisSign sign) {
        if (mouse_wheel_is_bound(sign)) {
            clear_mouse_wheel(sign);
        }
        assert(!mouse_wheel_is_bound(sign));

        m_binds.push_back(InputBind::make_mouse_wheel(action_id, sign));
    }


    bool button_is_bound(const Button button) const {
        const auto find = std::find_if(
            m_binds.begin(),
            m_binds.end(),
            [button](const InputBind& bind) {
                return (
                    bind.is_source(InputSource::Button) &&
                    button == bind.get_input()
                );
            }
        );

        return find != m_binds.end();
    }

    void clear() {
        m_binds.clear();
    }

    void clear_axis(const Axis axis, const AxisSign sign) {
        const auto find = std::find_if(
            m_binds.begin(),
            m_binds.end(),
            [axis, sign](const InputBind& bind) {
                return bind.is_axis(axis, sign);
            }
        );

        m_binds.erase(find);
    }

    void clear_button(const Button button) {
        const auto find = std::find_if(
            m_binds.begin(),
            m_binds.end(),
            [button](const InputBind& bind) {
                return (
                    bind.is_source(InputSource::Button) &&
                    button == bind.get_input()
                );
            }
        );

        m_binds.erase(find);
    }

    void clear_key(const Key key) {
        const auto find = std::find_if(
            m_binds.begin(),
            m_binds.end(),
            [key](const InputBind& bind) {
                return bind.is_key(key);
            }
        );

        m_binds.erase(find);
    }

    void clear_mouse_button(const Key key) {
        const auto find = std::find_if(
            m_binds.begin(),
            m_binds.end(),
            [key](const InputBind& bind) {
                return bind.is_mouse_button(key);
            }
        );

        m_binds.erase(find);
    }

    void clear_mouse_wheel(const AxisSign sign) {
        const auto find = std::find_if(
            m_binds.begin(),
            m_binds.end(),
            [sign](const InputBind& bind) {
                return bind.is_mouse_wheel(sign);
            }
        );

        m_binds.erase(find);
    }

    //  Gets all binds for a specific action
    const std::vector<InputBind>& get_binds() const {
        return m_binds;
    }

    //  Gets all binds for a specific action
    void get_binds(
        InputActionId action_id,
        std::vector<InputBind>& binds
    ) const {
        for (const InputBind& bind : m_binds) {
            if (bind.get_action_id() == action_id) {
                binds.push_back(bind);
            }
        }
    }

    //  Gets all binds for a specific action
    void get_binds(
        InputActionId action_id,
        InputSource source,
        std::vector<InputBind>& binds
    ) const {
        for (const InputBind& bind : m_binds) {
            if (bind.get_action_id() == action_id && bind.is_source(source)) {
                binds.push_back(bind);
            }
        }
    }

    //  Gets all binds of a specific source
    void get_binds(
        InputSource source,
        std::vector<InputBind>& binds
    ) const {
        for (const InputBind& bind : m_binds) {
            if (bind.is_source(source)) {
                binds.push_back(bind);
            }
        }
    }

    bool key_is_bound(const Key key) const {
        const auto find = std::find_if(
            m_binds.begin(),
            m_binds.end(),
            [key](const InputBind& bind) {
                return bind.is_key(key);
            }
        );

        return find != m_binds.end();
    }

    bool mouse_button_is_bound(const Button button) const {
        const auto find = std::find_if(
            m_binds.begin(),
            m_binds.end(),
            [button](const InputBind& bind) {
                return bind.is_mouse_button(button);
            }
        );

        return find != m_binds.end();
    }

    bool mouse_wheel_is_bound(const AxisSign sign) const {
        const auto find = std::find_if(
            m_binds.begin(),
            m_binds.end(),
            [sign](const InputBind& bind) {
                return bind.is_mouse_wheel(sign);
            }
        );

        return find != m_binds.end();
    }

    void unbind_axis(const Axis axis, const AxisSign sign) {
        m_binds.erase(
            std::remove_if(
                m_binds.begin(),
                m_binds.end(),
                [axis, sign](const InputBind& bind) {
                    return bind.is_axis(axis, sign);
                }
            ),
            m_binds.end()
        );
    }

    void unbind_button(const Button button) {
        m_binds.erase(
            std::remove_if(
                m_binds.begin(),
                m_binds.end(),
                [button](const InputBind& bind) {
                    return bind.is_button(button);
                }
            ),
            m_binds.end()
        );
    }

    void unbind_key(const Key key) {
        m_binds.erase(
            std::remove_if(
                m_binds.begin(),
                m_binds.end(),
                [key](const InputBind& bind) {
                    return bind.is_key(key);
                }
            ),
            m_binds.end()
        );
    }
};
}
