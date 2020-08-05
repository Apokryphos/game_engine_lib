#pragma once

#include "assets/spine_asset.hpp"
#include <cassert>
#include <future>
#include <optional>

namespace assets
{
using SpineAssetPromise = std::optional<std::promise<SpineAsset>>;
using SpineAssetFuture = std::future<SpineAsset>;

inline SpineAssetPromise make_spine_asset_promise() {
    return std::make_optional<std::promise<SpineAsset>>();
}

inline SpineAssetFuture get_spine_asset_future(SpineAssetPromise& promise) {
    assert(promise.has_value());
    return promise.value().get_future();
}
}
