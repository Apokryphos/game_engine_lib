#include "render_vk/framebuffers.hpp"
#include "render_vk/vulkan_swapchain.hpp"
#include <array>
#include <stdexcept>

namespace render_vk
{
//  ----------------------------------------------------------------------------
void create_framebuffers(
    VkDevice device,
    VkRenderPass render_pass,
    VulkanSwapchain& swapchain
) {
    swapchain.framebuffers.resize(swapchain.image_views.size());

    //  Create framebuffer for each image view
    for (size_t n = 0; n < swapchain.image_views.size(); ++n) {
        std::array<VkImageView, 1> attachments = {
            swapchain.image_views[n],
        };

        VkFramebufferCreateInfo framebuffer_info{};
        framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebuffer_info.renderPass = render_pass;
        framebuffer_info.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebuffer_info.pAttachments = attachments.data();
        framebuffer_info.width = swapchain.extent.width;
        framebuffer_info.height = swapchain.extent.height;
        framebuffer_info.layers = 1;

        if (vkCreateFramebuffer(
            device,
            &framebuffer_info,
            nullptr,
            &swapchain.framebuffers[n]
        ) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create framebuffer.");
        }
    }
}

//  ----------------------------------------------------------------------------
void create_framebuffers(
    VkDevice device,
    VkRenderPass render_pass,
    VkImageView depth_image_view,
    VulkanSwapchain& swapchain
) {
    swapchain.framebuffers.resize(swapchain.image_views.size());

    //  Create framebuffer for each image view
    for (size_t n = 0; n < swapchain.image_views.size(); ++n) {
        std::array<VkImageView, 2> attachments = {
            swapchain.image_views[n],
            depth_image_view
        };

        VkFramebufferCreateInfo framebuffer_info{};
        framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebuffer_info.renderPass = render_pass;
        framebuffer_info.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebuffer_info.pAttachments = attachments.data();
        framebuffer_info.width = swapchain.extent.width;
        framebuffer_info.height = swapchain.extent.height;
        framebuffer_info.layers = 1;

        if (vkCreateFramebuffer(
            device,
            &framebuffer_info,
            nullptr,
            &swapchain.framebuffers[n]
        ) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create framebuffer.");
        }
    }
}
}
