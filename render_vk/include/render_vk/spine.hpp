#pragma once

#include <string>
#include <glm/mat4x4.hpp>

namespace assets
{
struct TextureAsset;
}

namespace render_vk
{
struct SpineModel;

glm::mat4 calculate_transform(SpineModel& model);

std::unique_ptr<SpineModel> load_spine(
    const std::string& path,
    assets::TextureAsset& texture_asset
);
}
