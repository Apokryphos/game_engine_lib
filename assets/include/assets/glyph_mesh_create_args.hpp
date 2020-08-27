#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <vector>

namespace assets
{
struct GlyphMeshCreateArgs
{
    struct Glyph
    {
        uint32_t texture_id;
        glm::vec2 size;
        glm::vec3 position;
        glm::vec4 bg_color;
        glm::vec4 fg_color;
    };

    std::vector<Glyph> glyphs;
};
}
