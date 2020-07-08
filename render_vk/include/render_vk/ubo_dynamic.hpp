#pragma once

#include "render_vk/vulkan.hpp"
#include <glm/mat4x4.hpp>

namespace render_vk
{
//  Number of objects to create UBO data structs for
const size_t OBJECT_INSTANCES = 100;

//  Per-object UBO data (updated every object, every frame)
struct UboData
{
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
};

//  UBO data storage (CPU) for copying to GPU UBO dynamic buffer
struct UboDataDynamic
{
    UboData* data = nullptr;
};

//  UBO dynamic buffers
struct UniformBuffers
{
    //  Dynamic buffer alignment (bytes)
    size_t dynamic_align;
    //  Dynamic buffer size (bytes)
    size_t buffer_size;
    //  Dynamic uniform buffer
    VkBuffer dynamic = VK_NULL_HANDLE;
    //  Dynamic uniform buffer memory
    VkDeviceMemory dynamic_memory = VK_NULL_HANDLE;
    //  Mapped memory for UBO data (CPU -> GPU)
    void* dynamic_mapped = nullptr;
};

//  Copies UBO data from CPU to GPU
void copy_ubo_data_dynamic(
    const UboDataDynamic& ubo_dynamic,
    UniformBuffers& uniform_buffers
);

//  Unmaps and frees UBO dynamic data.
//  Destroys and frees UBO dynamic buffer.
void destroy_dynamic_uniform_buffer(
    VkDevice device,
    UniformBuffers& uniform_buffers,
    UboDataDynamic& ubo_dynamic
);

void prepare_uniform_buffers(
    VkPhysicalDevice physical_device,
    VkDevice device,
    UniformBuffers& uniform_buffers,
    UboDataDynamic& ubo_dynamic
);
}
