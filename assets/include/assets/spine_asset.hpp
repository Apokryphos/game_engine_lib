#pragma once

#include "assets/asset_id.hpp"
#include <string>

namespace spine
{
class AnimationStateData;
class Atlas;
class Skeleton;
class SkeletonData;
}

namespace assets
{
struct SpineAsset
{
    AssetId id         {0};
    AssetId texture_id {0};

    spine::AnimationStateData* anim_state_data {nullptr};
    spine::Atlas* atlas                        {nullptr};
    spine::Skeleton* skeleton                  {nullptr};
    spine::SkeletonData* skeleton_data         {nullptr};
};
}
