#pragma once

#include <algorithm>

namespace common
{
template <typename T>
class Gauge
{
    T m_min;
    T m_max;
    T m_value;

public:
    Gauge()
    : m_min(0),
      m_max(1) {
        fill();
    }

    Gauge(T max)
    : m_min(std::min(0, max)),
      m_max(std::max(0, max)) {
        fill();
    }

    Gauge(T min, T max)
    : m_min(std::min(min, max)),
      m_max(std::max(min, max)) {
        fill();
    }

    virtual ~Gauge() {}

    void drain() {
        set_value(m_min);
    }

    void fill() {
        set_value(m_max);
    }

    T get_max() const {
        return m_max;
    }

    T get_min() const {
        return m_min;
    }

    T get_value() const {
        return m_value;
    }

    bool is_empty() const {
        return m_value <= m_min;
    }

    bool is_full() const {
        return m_value >= m_max;
    }

    void set_max(T max) {
        m_min = std::min(m_min, max);
        m_max = std::max(m_min, max);
        set_value(m_value);
    }

    void set_min(T min) {
        m_min = std::min(min, m_max);
        m_max = std::max(min, m_max);
        set_value(m_value);
    }

    void set_value(T value) {
        m_value = std::max(m_min, std::min(value, m_max));
    }
};
}
