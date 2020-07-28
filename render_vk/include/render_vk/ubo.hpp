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
    glm::mat4 ortho;
};

//  Per-object UBO data (updated every object, every frame)
struct ObjectUbo
{
    glm::mat4 model;
    uint32_t texture_index;
};
}
