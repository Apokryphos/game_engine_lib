#pragma once

#include "common/string.hpp"
#include <rang.hpp>
#include <iostream>

namespace common
{
inline void log_debug(const std::string& str) {
    #ifdef DEBUG
    std::cout <<
        rang::style::bold << rang::fg::black <<
        str <<
        rang::style::reset << rang::fg::reset <<
        "\n";
    #endif
}

template <typename... Args>
inline void log_debug(const char* format, Args... args) {
    #ifdef DEBUG
    log_debug(stringf(format, args...));
    #endif
}

inline void log_error(const std::string& str) {
    std::cerr <<
        rang::fg::red <<
        str <<
        rang::fg::reset <<
        "\n";
}

template <typename... Args>
inline void log_error(const char* format, Args... args) {
    log_error(stringf(format, args...));
}

inline void log_help(const std::string& str) {
    std::cout << str << "\n";
}

template <typename... Args>
inline void log_help(const char* format, Args... args) {
    log_help(stringf(format, args...));
}

inline void log_info(const std::string& str) {
    std::cout <<
        rang::fg::green <<
        str <<
        rang::fg::reset <<
        "\n";
}

template <typename... Args>
inline void log_info(const char* format, Args... args) {
    log_info(stringf(format, args...));
}
}
