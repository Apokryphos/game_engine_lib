#pragma once

#include "assets/asset_id.hpp"

namespace assets
{
struct SpineAsset;

class SpineManager
{
public:
    SpineManager() = default;
    virtual ~SpineManager() = default;
    SpineManager(const SpineManager&) = delete;
    SpineManager& operator=(const SpineManager&) = delete;
    virtual const SpineAsset* get_asset(const AssetId id) const = 0;
};
}
