#pragma once

#include <string>

namespace platform
{
struct WindowOptions
{
    int width;
    int height;
    std::string title;
};

inline WindowOptions make_window_options(
    int width,
    int height,
    const std::string& title
) {
    return {
        width,
        height,
        title
    };
}
}
