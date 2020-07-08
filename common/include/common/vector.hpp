#pragma once

#include <algorithm>
#include <cassert>
#include <vector>

namespace common
{
/*	Removes the specified element from the vector by swapping it with the
    last element and then popping the last element. */
template <typename T>
void swap_remove(std::vector<T>& elements, const size_t index) {
    assert(index < elements.size());

    if (index + 1 != elements.size()) {
        std::swap(elements[index], elements.back());
    }

    elements.pop_back();
}
}