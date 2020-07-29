#pragma once

#include "render_vk/vulkan.hpp"

namespace render_vk
{
struct FrameDescriptorObjects
{
    //  The descriptor pool for this frame.
    VkDescriptorPool pool      = VK_NULL_HANDLE;
    //  The per-frame descriptor set for this frame.
    VkDescriptorSet frame_set  = VK_NULL_HANDLE;
    //  The per-object descriptor set for this frame.
    // VkDescriptorSet object_set = VK_NULL_HANDLE;
    //  The texture array descriptor set for this frame.
    VkDescriptorSet texture_set = VK_NULL_HANDLE;
};

struct FrameCommandObjects
{
    //  The command pool for this frame.
    VkCommandPool pool       = VK_NULL_HANDLE;
    //  The secondary command buffer for this frame.
    VkCommandBuffer buffer   = VK_NULL_HANDLE;
};

struct FrameSyncObjects
{
    //  Swapchain image is acquired.
    VkSemaphore image_acquired       = VK_NULL_HANDLE;
    //  Command buffers are finished processing.
    VkSemaphore present_ready        = VK_NULL_HANDLE;
    //  This frame is complete and resources can be reused.
    VkFence frame_complete           = VK_NULL_HANDLE;
};
}
