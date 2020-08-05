#pragma once

#include "assets/texture_asset_promise.hpp"
#include <string>

namespace assets
{
class AssetManager;

struct SpineLoadArgs
{
    //  Path to Spine assets folder
    std::string path;
    //  Used by AssetManager to schedule texture loading on a worker thread.
    TextureAssetFuture texture_future;
};
}
