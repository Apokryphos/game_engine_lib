#include "common/alloc.hpp"
#include "render_vk/vulkan.hpp"

using namespace common;

namespace render_vk
{
//  ----------------------------------------------------------------------------
//  Gets the minimum required alignment, in bytes, for uniform buffers
static VkDeviceSize gpu_device_align(VkPhysicalDevice physical_device) {
    VkPhysicalDeviceProperties device_props;
    vkGetPhysicalDeviceProperties(physical_device, &device_props);
    return device_props.limits.minUniformBufferOffsetAlignment;
}

//  ----------------------------------------------------------------------------
//  device_align = minUniformBufferOffsetAlignment (VkPhysicalDeviceProperties)
//  uniform_buffer_size = e.g. sizeof(UniformBufferObject)
static size_t gpu_dynamic_align(
    VkDeviceSize device_align,
    size_t uniform_buffer_size
) {
    return
        (uniform_buffer_size / device_align) * device_align +
        ((uniform_buffer_size % device_align) > 0 ? device_align : 0);
}

//  ----------------------------------------------------------------------------
void get_dynamic_buffer_align_and_size(
    VkPhysicalDevice physical_device,
    const size_t object_count,
    const size_t uniform_buffer_size,
    size_t& dynamic_align,
    size_t& buffer_size
) {
    const size_t device_align = gpu_device_align(physical_device);

    dynamic_align = gpu_dynamic_align(device_align, uniform_buffer_size);

    buffer_size = object_count * dynamic_align;
}
}
