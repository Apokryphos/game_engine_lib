#pragma once

#include "volk.h"
#include <cassert>

#define VK_CHECK_RESULT(f)          \
{                                   \
    VkResult result = (f);          \
    assert(result == VK_SUCCESS);   \
}
