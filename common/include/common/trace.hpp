#pragma once

#include "common/log.hpp"
#include <map>
#include <mutex>
#include <thread>
#include <string>
#include <signal.h>

namespace common
{
static std::mutex s_trace_depth_mutex;

//  Depth stored by thread ID
static std::map<std::thread::id, uint32_t> s_trace_depth;

static std::string get_tab_string() {
    uint32_t depth = s_trace_depth[std::this_thread::get_id()];

    std::string tabs;
    for (uint32_t n = 0; n < depth; ++n) {
        tabs += "    ";
    }
    return tabs;
}

void inline breakpoint() {
    raise(SIGTRAP);
}

class Trace
{
    std::string m_func_name;

public:
    Trace(std::string func_name)
    : m_func_name(func_name) {
        entry(m_func_name);
    }

    ~Trace() {
        exit(m_func_name);
    }

    static void entry(std::string func_name) {
        std::lock_guard<std::mutex> lock(s_trace_depth_mutex);

        log_trace(
            "%s{ %s (%p)",
            get_tab_string().c_str(),
            func_name.c_str(),
            std::this_thread::get_id()
        );

        ++s_trace_depth[std::this_thread::get_id()];
    }

    static void exit(std::string func_name) {
        std::lock_guard<std::mutex> lock(s_trace_depth_mutex);
        --s_trace_depth[std::this_thread::get_id()];

        log_trace(
            "%s} %s (%p)",
            get_tab_string().c_str(),
            func_name.c_str(),
            std::this_thread::get_id()
        );
    }
};

inline std::string method_name(const std::string& pretty_function)
{
    size_t colons = pretty_function.find("::");
    size_t begin = pretty_function.substr(0,colons).rfind(" ") + 1;
    size_t end = pretty_function.rfind("(") - begin;

    return pretty_function.substr(begin,end) + "()";
}

#define __METHOD_NAME__ method_name(__PRETTY_FUNCTION__)

#define TRACE() Trace _trace(__METHOD_NAME__);
}
