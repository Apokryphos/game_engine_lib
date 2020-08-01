#pragma once

#include "common/trace.hpp"
#include "volk.h"
#include <cassert>

#define VK_CHECK_RESULT(f)          \
{                                   \
    common::Trace::entry(#f);       \
    VkResult result = (f);          \
    assert(result == VK_SUCCESS);   \
    common::Trace::exit(#f);        \
}
