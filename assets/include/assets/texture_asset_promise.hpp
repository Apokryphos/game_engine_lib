#pragma once

#include "assets/texture_asset.hpp"
#include <future>
#include <optional>

namespace assets
{
using TextureAssetPromise = std::optional<std::promise<TextureAsset>>;
}
