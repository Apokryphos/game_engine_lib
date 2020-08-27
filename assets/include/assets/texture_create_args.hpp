#pragma once

#include "assets/texture_address_mode.hpp"
#include "assets/texture_filter.hpp"

namespace assets
{
struct TextureCreateArgs
{
    // Generate mipmaps
    bool mipmaps {true};
    TextureAddressMode address_mode {TextureAddressMode::Clamp};
    TextureFilter mag_filter {TextureFilter::Linear};
    TextureFilter min_filter {TextureFilter::Linear};
};
}
