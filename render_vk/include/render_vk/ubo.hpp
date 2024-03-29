#pragma once

#include "render_vk/vulkan.hpp"
#include <glm/mat4x4.hpp>
#include <cstdint>

namespace render_vk
{
//  Per-frame UBO data (updated once a frame)
struct FrameUbo
{
    glm::mat4 view;
    glm::mat4 proj;
    glm::mat4 ortho_view;
    glm::mat4 ortho_proj;
};

//  Per-instance UBO data (updated every frame)
struct alignas(16) GlyphUbo
{
    alignas(16) glm::mat4 model;
    alignas(16) glm::vec4 bg_color;
    alignas(16) glm::vec4 fg_color;
    alignas(4) uint32_t texture_index;
};

//  Per-object UBO data (updated every object, every frame)
struct ObjectUbo
{
    glm::mat4 model;
    uint32_t texture_index;
};

struct SpineUbo
{
    //  Transform matrix from parent bone
    glm::mat4 transform;
};
}
