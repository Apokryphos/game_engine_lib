#pragma once

#include "assets/asset_id.hpp"
#include "assets/spine_asset.hpp"
#include <glm/vec3.hpp>
#include <cstdint>
#include <vector>

namespace render
{
struct SpineSpriteBatch
{
    assets::AssetId spine_id;
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> sizes;
    assets::SpineAsset asset;
};
}
