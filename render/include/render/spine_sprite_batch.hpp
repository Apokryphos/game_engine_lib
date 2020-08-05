#pragma once

#include "assets/asset_id.hpp"
#include <glm/vec3.hpp>
#include <cstdint>
#include <vector>

namespace spine
{
class Atlas;
class Skeleton;
class SkeletonData;
}

namespace render
{
struct SpineSpriteBatch
{
    assets::AssetId texture_id;
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> sizes;
    spine::Atlas* atlas;
    spine::Skeleton* skeleton;
    spine::SkeletonData* skeleton_data;
};
}
