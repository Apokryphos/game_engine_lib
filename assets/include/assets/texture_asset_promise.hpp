#pragma once

#include "assets/texture_asset.hpp"
#include <cassert>
#include <future>
#include <optional>

namespace assets
{
using TextureAssetPromise = std::optional<std::promise<TextureAsset>>;
using TextureAssetFuture = std::future<TextureAsset>;

inline TextureAssetPromise make_texture_asset_promise() {
    return std::make_optional<std::promise<TextureAsset>>();
}

inline TextureAssetFuture get_texture_asset_future(TextureAssetPromise& promise) {
    assert(promise.has_value());
    return promise.value().get_future();
}
}
