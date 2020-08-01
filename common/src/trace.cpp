#include <cstdint>
#include <mutex>

namespace common
{
static std::mutex s_trace_depth_mutex;
static uint32_t s_trace_depth {0};
}
