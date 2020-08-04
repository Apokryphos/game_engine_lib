#pragma once

#include "render_vk/texture_id.hpp"
#include <string>

namespace assets
{
class AssetManager;
}

namespace render_vk
{
void load_spine(
    const TextureId texture_id,
    const std::string& path,
    assets::AssetManager& asset_mgr
);
}
