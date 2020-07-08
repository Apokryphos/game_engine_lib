#include "common/log.hpp"
#include "render_vk/queue_family.hpp"
#include "render_vk/vulkan.hpp"

using namespace common;

namespace render_vk
{
//  ----------------------------------------------------------------------------
bool create_command_pool(
    VkDevice device,
    VkPhysicalDevice physical_device,
    VkSurfaceKHR surface,
    VkCommandPool& command_pool
) {
    QueueFamilyIndices queue_family_indices = find_queue_families(physical_device, surface);

    VkCommandPoolCreateInfo pool_info{};
    pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    pool_info.queueFamilyIndex = queue_family_indices.graphics_family.value();
    pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    if (vkCreateCommandPool(device, &pool_info, nullptr, &command_pool) != VK_SUCCESS) {
        log_debug("Failed to create command pool.");
        return false;
    }

    return true;
}
}
