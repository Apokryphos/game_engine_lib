#pragma once

#include <algorithm>
#include <cstdint>

namespace common
{
template <typename T>
inline T clamp(T value, const T min, const T max) {
    return std::min(max, std::max(value, min));
}

template <typename T>
T next_power_of_two(T v) {
    //  If the specified value is not a power of two, then the next power of
    //  two will be returned. Otherwise the value will not change.
    v--;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    v++;

    return v;
}
}
