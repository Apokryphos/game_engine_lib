#pragma once

#include "render_vk/vulkan.hpp"
#include <glm/mat4x4.hpp>

namespace render_vk
{
//  Per-frame UBO data (updated once a frame)
struct FrameUbo
{
    glm::mat4 view;
    glm::mat4 proj;
};

//  Per-object UBO data (updated every object, every frame)
struct ObjectUbo
{
    glm::mat4 model;
};
}