#pragma once

#include "assets/asset_id.hpp"
#include <glm/vec3.hpp>
#include <cstdint>
#include <vector>

namespace render
{
struct SpineSpriteBatch
{
    //  Spine model used by this batch
    assets::AssetId spine_id;
    //  Texture used by this batch
    assets::AssetId texture_id;
    //  Position of each object
    std::vector<glm::vec3> positions;
    //  Size of each object
    std::vector<glm::vec3> sizes;
};
}
