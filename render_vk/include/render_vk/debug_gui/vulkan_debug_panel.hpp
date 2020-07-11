#pragma once

#include "render_vk/vulkan.hpp"
#include <functional>

namespace engine
{
class Game;
}

namespace render_vk
{
std::function<void (engine::Game&)> make_vulkan_debug_panel_function(
    VkInstance instance
);
}
