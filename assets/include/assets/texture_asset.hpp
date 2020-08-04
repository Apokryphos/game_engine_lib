#pragma once

#include "assets/asset_id.hpp"
#include <string>

namespace assets
{
struct TextureAsset
{
    AssetId id      {0};
    uint32_t width  {0};
    uint32_t height {0};
};
}
