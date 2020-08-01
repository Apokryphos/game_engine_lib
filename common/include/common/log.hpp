#pragma once

#include "common/string.hpp"
#include <rang.hpp>
#include <fstream>
#include <iostream>
#include <memory>
#include <mutex>

namespace common
{
struct Log
{
    uint32_t line {1};
    std::ofstream file;
};

extern std::unique_ptr<Log> s_log;

void initialize_log(const std::string& path);

inline void log(
    const std::string& str,
    rang::fg color,
    rang::style style,
    bool error = false
) {
    //  Standard output / error
    static std::mutex cout_mutex;
    {
        std::lock_guard<std::mutex> lock(cout_mutex);

        (error ? std::cerr : std::cout) <<
            style << color <<
            (s_log == nullptr ? "" : (std::to_string(s_log->line) + ": ")) <<
            str <<
            rang::style::reset << rang::fg::reset <<
            "\n";
    }

    if (s_log == nullptr) {
        return;
    }

    //  Log file
    static std::mutex file_mutex;
    {
        std::lock_guard<std::mutex> lock(file_mutex);

        //  Write to file and flush immediately in case application locks up.
        s_log->file <<
            (s_log == nullptr ? "" : (std::to_string(s_log->line) + ": ")) <<
            str <<
            std::endl;

        ++s_log->line;
    }
}

inline void log_debug(const std::string& str) {
    #ifdef DEBUG
    log(str, rang::fg::black, rang::style::bold);
    #endif
}

template <typename... Args>
inline void log_debug(const char* format, Args... args) {
    #ifdef DEBUG
    log_debug(stringf(format, args...));
    #endif
}

inline void log_error(const std::string& str) {
    log(str, rang::fg::red, rang::style::bold, true);
}

template <typename... Args>
inline void log_error(const char* format, Args... args) {
    log_error(stringf(format, args...));
}

inline void log_help(const std::string& str) {
    log(str, rang::fg::gray, rang::style::dim);
}

template <typename... Args>
inline void log_help(const char* format, Args... args) {
    log_help(stringf(format, args...));
}

inline void log_info(const std::string& str) {
    log(str, rang::fg::green, rang::style::bold);
}

template <typename... Args>
inline void log_info(const char* format, Args... args) {
    log_info(stringf(format, args...));
}

inline void log_trace(const std::string& str) {
    log(str, rang::fg::magenta, rang::style::bold);
}

template <typename... Args>
inline void log_trace(const char* format, Args... args) {
    log_trace(stringf(format, args...));
}

inline void log_warn(const std::string& str) {
    log(str, rang::fg::yellow, rang::style::bold);
}

template <typename... Args>
inline void log_warn(const char* format, Args... args) {
    log_warn(stringf(format, args...));
}
}
