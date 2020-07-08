#include "render_vk/queue_family.hpp"
#include "render_vk/vulkan.hpp"
#include <vector>

namespace render_vk
{
//  ----------------------------------------------------------------------------
QueueFamilyIndices find_queue_families(
    VkPhysicalDevice device,
    VkSurfaceKHR surface
) {
    QueueFamilyIndices indices;

    uint32_t queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, nullptr);

    std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_families.data());

    int n = 0;
    for (const auto& queue_family : queue_families) {
        if (queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphics_family = n;
        }

        VkBool32 present_support = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, n, surface, &present_support);
        if (present_support) {
            indices.present_family = n;
        }

        //  TODO: Separate transfer queue (optional)
        //  Any queue family with VK_QUEUE_GRAPHICS_BIT or VK_QUEUE_COMPUTE_BIT
        //  implicitly supports VK_QUEUE_TRANSFER_BIT operations.
        //  https://vulkan-tutorial.com/en/Vertex_buffers/Staging_buffer

        if (indices.is_complete()) {
            break;
        }

        ++n;
    }

    return indices;
}
}
