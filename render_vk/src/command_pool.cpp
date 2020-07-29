#include "render_vk/debug_utils.hpp"
#include "render_vk/queue_family.hpp"
#include "render_vk/vulkan.hpp"
#include <stdexcept>
#include <vector>

namespace render_vk
{
//  ----------------------------------------------------------------------------
static uint32_t find_graphics_queue_family_index(VkPhysicalDevice device) {
    uint32_t queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, nullptr);

    std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_families.data());

    int n = 0;
    for (const auto& queue_family : queue_families) {
        if (queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            return n;
        }

        ++n;
    }

    throw std::runtime_error("No graphics family queue found.");
}

//  ----------------------------------------------------------------------------
void create_command_pool(
    VkDevice device,
    VkPhysicalDevice physical_device,
    VkCommandPool& command_pool,
    const char* debug_name
) {
    const uint32_t queue_family_index = find_graphics_queue_family_index(physical_device);

    VkCommandPoolCreateInfo pool_info{};
    pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    pool_info.queueFamilyIndex = queue_family_index;
    pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    if (vkCreateCommandPool(device, &pool_info, nullptr, &command_pool) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create command pool.");
    }

    set_debug_name(device, VK_OBJECT_TYPE_COMMAND_POOL, command_pool, debug_name);
}

//  ----------------------------------------------------------------------------
void create_transient_command_pool(
    VkDevice device,
    VkPhysicalDevice physical_device,
    VkCommandPool& command_pool,
    const char* debug_name
) {
    const uint32_t queue_family_index = find_graphics_queue_family_index(physical_device);

    VkCommandPoolCreateInfo pool_info{};
    pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    pool_info.queueFamilyIndex = queue_family_index;
    pool_info.flags =
        VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT |
        VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;

    if (vkCreateCommandPool(device, &pool_info, nullptr, &command_pool) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create transient command pool.");
    }

    set_debug_name(device, VK_OBJECT_TYPE_COMMAND_POOL, command_pool, debug_name);
}
}
