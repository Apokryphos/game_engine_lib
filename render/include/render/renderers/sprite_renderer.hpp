#pragma once

#include <glm/vec2.hpp>
#include <cstdint>
#include <vector>

namespace render
{
class SpriteRenderer
{
public:
    void draw_sprites(
        std::vector<glm::vec2>& position,
        std::vector<uint32_t>& texture_id
    );
};
}
