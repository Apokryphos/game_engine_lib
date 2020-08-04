#pragma once

#include "assets/texture_asset_promise.hpp"
#include <string>

namespace assets
{
struct TextureLoadArgs
{
    std::string path;
    TextureAssetPromise promise;
};
}
