#pragma once

#include <functional>
#include <map>

namespace common
{
template <typename... Args>
class Signal
{
    mutable int m_id;
    mutable std::map<int, std::function<void(Args...)>> m_slots;

public:
    //  ------------------------------------------------------------------------
    Signal()
    : m_id(0) {
    }

    //  ------------------------------------------------------------------------
    Signal(Signal const& signal)
    : m_id(0) {
    }

    //  ------------------------------------------------------------------------
    Signal& operator=(Signal const& signal) {
        disconnectAll();
    }

    //  ------------------------------------------------------------------------
    int connect(std::function<void(Args...)> const& slot) const {
        m_slots.insert(std::make_pair(++m_id, slot));
        return m_id;
    }

    //  ------------------------------------------------------------------------
    template <typename T>
    int connect(T* instance, void (T::*func)(Args...)) {
        return connect([=](Args... args) {
            (instance->*func)(args...);
        });
    }

    //  ------------------------------------------------------------------------
    template <typename T>
    int connect(T* instance, void (T::*func)(Args...) const) {
        return connect([=](Args... args) {
            (instance->*func)(args...);
        });
    }

    //  ------------------------------------------------------------------------
    void disconnect(int id) const {
        m_slots.erase(id);
    }

    //  ------------------------------------------------------------------------
    void disconnect_all() const {
        m_slots.clear(m_id);
    }

    //  ------------------------------------------------------------------------
    void emit(Args... p) {
        for (auto pair : m_slots) {
            pair.second(p...);
        }
    }
};
}
