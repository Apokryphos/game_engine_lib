#pragma once

#include "common/log.hpp"
#include <chrono>
#include <cstdint>
#include <map>
#include <numeric>
#include <string>
#include <vector>

namespace common
{
class Stopwatch
{
    struct Entry
    {
        std::chrono::steady_clock::time_point start_time;
        std::vector<uint64_t> samples;

        double average() const {
            return std::accumulate(
                samples.begin(),
                samples.end(),
                0.0
            ) / samples.size();
        }
    };

    std::map<std::string, Entry> m_entries;

public:
    ~Stopwatch() {
        for (const auto& pair : m_entries) {
            const Entry& entry = pair.second;
            const double avg = entry.average();
            log_debug(
                "%30s: %0.2f μs | %0.4f ms | %d samples",
                pair.first.c_str(),
                avg,
                avg / 1000.0,
                entry.samples.size()
            );
        }
    }

    void start(const std::string& title) {
        m_entries[title].start_time = std::chrono::steady_clock::now();
    }

    void stop(const std::string& title) {
        const auto now = std::chrono::steady_clock::now();

        Entry& entry = m_entries.at(title);
        const auto elapsed =
            std::chrono::duration_cast<std::chrono::microseconds>(now - entry.start_time).count();
        entry.samples.push_back(elapsed);
    }
};
}
