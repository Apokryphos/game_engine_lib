#pragma once

#include <glm/vec2.hpp>
#include <cstdint>
#include <vector>

namespace render
{
struct SpriteBatch
{
    uint32_t texture_id;
    std::vector<glm::vec2> positions;
    std::vector<glm::vec2> sizes;
};
}
