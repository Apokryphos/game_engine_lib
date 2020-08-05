#pragma once

#include "assets/texture_asset_promise.hpp"
#include <string>

namespace assets
{
struct TextureLoadArgs
{
    //  Path to texture file
    std::string path;
    //  Optional promise fulfilled after texture has loaded
    TextureAssetPromise promise;
};
}
