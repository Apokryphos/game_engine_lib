#pragma once

#include "render_vk/vulkan.hpp"
#include <glm/vec2.hpp>
#include <vector>

namespace render_vk
{
struct SpritePipeline
{
    VkPipelineLayout layout;
    VkPipeline pipeline;
};
}
