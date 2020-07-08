#pragma once

#include "input/axis_sign.hpp"
#include "input/input_action_id.hpp"
#include "input/input_source.hpp"
#include "input/input_type.hpp"
#include "input/input_types.hpp"
#include <cassert>
#include <tuple>

namespace input
{
//  A single binding between an input action and source
class InputBind
{
    Input m_input;
    InputSource m_source;
    AxisSign m_sign;
    InputActionId m_action_id;

public:
    InputBind(
        InputActionId action_id,
        Input input,
        InputSource source,
        AxisSign sign
    )
    : m_input(input),
      m_source(source),
      m_sign(sign),
      m_action_id(action_id) {
        assert(action_id != INPUT_ACTION_ID_UNASSIGNED);
    }

    InputActionId get_action_id() const {
        return m_action_id;
    }

    AxisSign get_axis_sign() const {
        return m_sign;
    }

    Input get_input() const {
        return m_input;
    }

    InputSource get_source() const {
        return m_source;
    }

    bool is_axis(const Axis axis, const AxisSign sign) const {
        return (
            m_source == InputSource::Axis &&
            m_input == axis &&
            m_sign == sign
        );
    }

    bool is_button(const Button button) const {
        return (
            m_source == InputSource::Button &&
            m_input == button
        );
    }

    bool is_key(const Key key) const {
        return (
            m_source == InputSource::Key &&
            m_input == key
        );
    }

    bool is_mouse_button(const Button button) const {
        return (
            m_source == InputSource::MouseButton &&
            m_input == button
        );
    }

    bool is_source(InputSource source) const {
        return m_source == source;
    }

    static inline InputBind make_axis(
        InputActionId action_id,
        Axis axis,
        AxisSign sign
    ) {
        return {
            action_id,
            axis,
            InputSource::Axis,
            sign
        };
    }

    static inline InputBind make_button(InputActionId action_id, Button button) {
        return {
            action_id,
            button,
            InputSource::Button,
            AxisSign::None
        };
    }

    static inline InputBind make_key(InputActionId action_id, Key key) {
        return {
            action_id,
            key,
            InputSource::Key,
            AxisSign::None
        };
    }

    static inline InputBind make_mouse_button(InputActionId action_id, Button button) {
        return {
            action_id,
            button,
            InputSource::MouseButton,
            AxisSign::None
        };
    }

    inline bool operator==(const InputBind& bind) {
        return (
            m_action_id == bind.m_action_id &&
            m_input == bind.m_input &&
            m_source == bind.m_source &&
            m_sign == bind.m_sign
        );
    }

    inline bool operator!=(const InputBind& bind) {
        return !(*this == bind);
    }

    inline bool operator<(const InputBind& bind) {
        return (
            std::tie(m_action_id, m_input, m_sign, m_source) <
            std::tie(bind.m_action_id, m_input, bind.m_sign, bind.m_source)
        );
    }
};
}
