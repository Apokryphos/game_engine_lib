#include "common/log.hpp"
#include <chrono>
#include <iomanip>

namespace common
{
std::unique_ptr<Log> s_log = nullptr;

//  ----------------------------------------------------------------------------
void initialize_log(const std::string& path) {
    auto log = std::make_unique<Log>();
    log->file.open(path);

    if (log->file.good()) {
        //  Add timestamp at top of log file
        const auto now = std::chrono::system_clock::now();
        const std::time_t timestamp = std::chrono::system_clock::to_time_t(now);
        log->file <<
            "Log file created " <<
            std::put_time(std::localtime(&timestamp), "%F %T") <<
            ".\n";

        s_log = std::move(log);
    } else {
        log_error("Failed to open log file '%s'.", path.c_str());
    }
}
}
