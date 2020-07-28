#include "common/log.hpp"

namespace common
{
std::unique_ptr<Log> s_log = nullptr;

//  ----------------------------------------------------------------------------
void initialize_log(const std::string& path) {
    auto log = std::make_unique<Log>();
    log->file.open(path);

    if (log->file.good()) {
        s_log = std::move(log);
    } else {
        log_error("Failed to open log file '%s'.", path.c_str());
    }
}
}
