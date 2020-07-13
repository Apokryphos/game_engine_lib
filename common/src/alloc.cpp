#include <malloc.h>
#include <stdlib.h>

namespace common
{
//  ----------------------------------------------------------------------------
//  Wrapper functions for aligned memory allocation.
//  Requires alignment to be a power of two.
void* aligned_alloc(size_t size, size_t alignment)
{
	void *data = nullptr;

    #if defined(_MSC_VER) || defined(__MINGW32__)
    data = _aligned_malloc(size, alignment);
    #else
    int res = posix_memalign(&data, alignment, size);
    if (res != 0) {
        data = nullptr;
    }
    #endif

	return data;
}
}
