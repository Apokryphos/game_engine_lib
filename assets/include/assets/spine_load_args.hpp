#pragma once

#include "assets/spine_asset_promise.hpp"
#include <string>

namespace assets
{
class AssetManager;

struct SpineLoadArgs
{
    AssetManager* asset_mgr;
    std::string path;
    SpineAssetPromise promise;
};
}
