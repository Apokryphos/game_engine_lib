#pragma once

#include "render/texture_filter.hpp"

namespace render
{
struct TextureCreateArgs
{
    // Generate mipmaps
    bool mipmaps {true};
    TextureFilter mag_filter {TextureFilter::Linear};
    TextureFilter min_filter {TextureFilter::Linear};
};
}
