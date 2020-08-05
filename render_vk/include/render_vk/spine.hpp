#pragma once

#include <string>

namespace assets
{
struct TextureAsset;
}

namespace render_vk
{
struct SpineModel;

std::unique_ptr<SpineModel> load_spine(
    const std::string& path,
    assets::TextureAsset& texture_asset
);
}
