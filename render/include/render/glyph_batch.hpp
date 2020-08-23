#pragma once

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <cstdint>
#include <vector>

namespace render
{
struct GlyphBatch
{
    uint32_t texture_id;
    std::vector<glm::vec3> positions;
    std::vector<glm::vec4> bg_colors;
    std::vector<glm::vec4> fg_colors;
    std::vector<glm::vec3> sizes;
};
}
