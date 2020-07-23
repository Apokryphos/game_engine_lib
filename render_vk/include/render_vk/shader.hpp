#pragma once

#include "render_vk/vulkan.hpp"
#include <string>
#include <vector>

namespace render_vk
{
std::vector<char> read_file(const std::string& filename);

VkShaderModule create_shader_module(
    VkDevice device,
    const std::vector<char>& code
);
}
