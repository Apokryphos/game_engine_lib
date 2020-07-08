#include "common/alloc.hpp"
#include "common/math.hpp"
#include "render_vk/buffer.hpp"
#include "render_vk/ubo_dynamic.hpp"
#include "render_vk/vulkan.hpp"
#include <cstring>

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
static void get_dynamic_buffer_align_and_size(
    VkPhysicalDevice physical_device,
    const size_t object_count,
    const size_t uniform_buffer_size,
    size_t& dynamic_align,
    size_t& buffer_size
) {
    const size_t device_align = gpu_device_align(physical_device);

    dynamic_align = gpu_dynamic_align(device_align, uniform_buffer_size);

    //  aligned_alloc requires that alignment must be a power of two
    dynamic_align = next_power_of_two(dynamic_align);

    buffer_size = uniform_buffer_size * object_count * dynamic_align;
}

//  ----------------------------------------------------------------------------
void unmap_ubo_data(
    const UniformBuffers& uniform_buffers,
    UboDataDynamic& ubo_dynamic
) {
    assert(ubo_dynamic.data == nullptr);
    ubo_dynamic.data = (UboData*)common::aligned_alloc(
        uniform_buffers.buffer_size,
        uniform_buffers.dynamic_align
    );
}

//  ----------------------------------------------------------------------------
void allocate_ubo_data(
    const UniformBuffers& uniform_buffers,
    UboDataDynamic& ubo_dynamic
) {
    assert(ubo_dynamic.data == nullptr);
    ubo_dynamic.data = (UboData*)common::aligned_alloc(
        uniform_buffers.buffer_size,
        uniform_buffers.dynamic_align
    );
    assert(ubo_dynamic.data != nullptr);
}

//  ----------------------------------------------------------------------------
//  Copies UBO data to dynamic uniform buffer
void copy_ubo_data_dynamic(
    const UboDataDynamic& ubo_dynamic,
    UniformBuffers& uniform_buffers
) {
    memcpy(
        uniform_buffers.dynamic_mapped,
        ubo_dynamic.data,
        uniform_buffers.buffer_size
    );
}

//  ----------------------------------------------------------------------------
void free_ubo_data(UboDataDynamic& ubo_dynamic) {
    if (ubo_dynamic.data != nullptr) {
        free(ubo_dynamic.data);
        ubo_dynamic.data = nullptr;
    }
}

//  ----------------------------------------------------------------------------
void map_ubo_data(VkDevice device, UniformBuffers& uniform_buffers) {
    assert(uniform_buffers.dynamic_memory != VK_NULL_HANDLE);
    assert(uniform_buffers.dynamic_mapped == nullptr);
    vkMapMemory(
        device,
        uniform_buffers.dynamic_memory,
        0,
        uniform_buffers.buffer_size,
        0,
        &uniform_buffers.dynamic_mapped
    );
    assert(uniform_buffers.dynamic_mapped != nullptr);
}

//  ----------------------------------------------------------------------------
void unmap_ubo_data(VkDevice device, UniformBuffers& uniform_buffers) {
    if (uniform_buffers.dynamic_mapped != VK_NULL_HANDLE) {
        vkUnmapMemory(device, uniform_buffers.dynamic_memory);
        uniform_buffers.dynamic_mapped = VK_NULL_HANDLE;
    }
}

//  ----------------------------------------------------------------------------
//  Unmaps and frees UBO dynamic data.
//  Destroys and frees UBO dynamic buffer.
void destroy_dynamic_uniform_buffer(
    VkDevice device,
    UniformBuffers& uniform_buffers,
    UboDataDynamic& ubo_dynamic
) {
    unmap_ubo_data(device, uniform_buffers);
    free_ubo_data(ubo_dynamic);

    vkDestroyBuffer(device, uniform_buffers.dynamic, nullptr);
    vkFreeMemory(device, uniform_buffers.dynamic_memory, nullptr);

    uniform_buffers.dynamic = VK_NULL_HANDLE;
    uniform_buffers.dynamic_memory = VK_NULL_HANDLE;
}

//  ----------------------------------------------------------------------------
void prepare_uniform_buffers(
    VkPhysicalDevice physical_device,
    VkDevice device,
    UniformBuffers& uniform_buffers,
    UboDataDynamic& ubo_dynamic
) {
    //  Unmap and free the data buffer
    unmap_ubo_data(device, uniform_buffers);
    free_ubo_data(ubo_dynamic);

    //  Determine alignment and calculate buffer size
    get_dynamic_buffer_align_and_size(
        physical_device,
        OBJECT_INSTANCES,
        sizeof(UboData),
        uniform_buffers.dynamic_align,
        uniform_buffers.buffer_size
    );

    //  Allocate data buffer (where UBO data will be stored in RAM)
    allocate_ubo_data(uniform_buffers, ubo_dynamic);

    //  Create UBO dynamic uniform buffer (GPU)
    create_buffer(
        physical_device,
        device,
        uniform_buffers.buffer_size,
        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        uniform_buffers.dynamic,
        uniform_buffers.dynamic_memory
    );

    //  Map UBO buffer (RAM -> GPU)
    map_ubo_data(device, uniform_buffers);
}
}
