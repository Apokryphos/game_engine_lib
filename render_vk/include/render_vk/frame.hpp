#pragma once

#include "render_vk/vulkan.hpp"
#include <vector>

namespace render_vk
{
struct Frame
{
    //  Each frame has a separate command pool, as vkResetCommandPool will
    //  reset ALL frames in the pool, including any in use.
    VkCommandPool                   cmd_pool;
    std::vector<VkCommandBuffer>    cmd_buffers;
    VkFramebuffer                   framebuffer;
};
}
