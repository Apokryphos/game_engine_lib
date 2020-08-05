#pragma once

#include "assets/texture_filter.hpp"

namespace assets
{
struct TextureCreateArgs
{
    // Generate mipmaps
    bool mipmaps {true};
    TextureFilter mag_filter {TextureFilter::Linear};
    TextureFilter min_filter {TextureFilter::Linear};
};
}
