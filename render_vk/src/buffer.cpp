#include "render_vk/command_buffer.hpp"
#include "render_vk/vulkan.hpp"
#include <cstring>
#include <stdexcept>

namespace render_vk
{
//  ----------------------------------------------------------------------------
uint32_t find_memory_type(
    VkPhysicalDevice physical_device,
    uint32_t type_filter,
    VkMemoryPropertyFlags properties
) {
    VkPhysicalDeviceMemoryProperties mem_properties;
    vkGetPhysicalDeviceMemoryProperties(physical_device, &mem_properties);

    for (uint32_t n = 0; n < mem_properties.memoryTypeCount; ++n) {
        if ((type_filter & (1 << n)) &&
            (mem_properties.memoryTypes[n].propertyFlags & properties)
        ) {
            return n;
        }
    }

    throw std::runtime_error("Failed to find suitable memory type.");
}

//  ----------------------------------------------------------------------------
static void allocate_buffer(
    VkPhysicalDevice physical_device,
    VkDevice device,
    VkBuffer buffer,
    VkMemoryPropertyFlags properties,
    VkDeviceMemory& buffer_memory
) {
    VkMemoryRequirements mem_requirements;
    vkGetBufferMemoryRequirements(device, buffer, &mem_requirements);

    VkMemoryAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    alloc_info.allocationSize = mem_requirements.size;
    alloc_info.memoryTypeIndex = find_memory_type(
        physical_device,
        mem_requirements.memoryTypeBits,
        properties
    );

    if (vkAllocateMemory(device, &alloc_info, nullptr, &buffer_memory) != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate vertex buffer memory.");
    }

    vkBindBufferMemory(device, buffer, buffer_memory, 0);
}

//  ----------------------------------------------------------------------------
void create_buffer(
    VkPhysicalDevice physical_device,
    VkDevice device,
    VkDeviceSize size,
    VkBufferUsageFlags usage,
    VkMemoryPropertyFlags properties,
    VkBuffer& buffer,
    VkDeviceMemory& buffer_memory
) {
    VkBufferCreateInfo buffer_info{};
    buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer_info.size = size;
    buffer_info.usage = usage;
    buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(device, &buffer_info, nullptr, &buffer) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create buffer.");
    }

    allocate_buffer(
        physical_device,
        device,
        buffer,
        properties,
        buffer_memory
    );
}

//  ----------------------------------------------------------------------------
void copy_buffer(
    VkDevice device,
    VkQueue transfer_queue,
    VkCommandPool command_pool,
    VkBuffer src,
    VkBuffer dst,
    VkDeviceSize size
) {
    //  TODO: Use separate command pool for short-lived buffers so
    //  driver can apply memory allocation optimizations.
    //  Use VK_COMMAND_POOL_CREATE_TRANSIENT_BIT when creating command pool.
    VkCommandBuffer command_buffer = begin_single_time_commands(device, command_pool);

    VkBufferCopy copy_region{};
    copy_region.srcOffset = 0; // Optional
    copy_region.dstOffset = 0; // Optional
    copy_region.size = size;
    vkCmdCopyBuffer(command_buffer, src, dst, 1, &copy_region);

    end_single_time_commands(
        device,
        command_pool,
        command_buffer,
        transfer_queue
    );
}
}
