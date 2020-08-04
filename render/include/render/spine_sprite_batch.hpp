#pragma once

#include <glm/vec3.hpp>
#include <cstdint>
#include <vector>

namespace render
{
struct SpineSpriteBatch
{
    uint32_t texture_id;
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> sizes;
};
}
