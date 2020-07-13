#pragma once

#include <stdlib.h>

namespace common
{
//  ----------------------------------------------------------------------------
//  Wrapper functions for aligned memory allocation.
//  Requires alignment to be a power of two.
void* aligned_alloc(size_t size, size_t alignment);
}
