#include "common/log.hpp"
#include "render_vk/buffer.hpp"
#include "render_vk/uniform.hpp"
#include "render_vk/vulkan.hpp"
#include "render_vk/vulkan_swapchain.hpp"
#include <array>
#include <stdexcept>

using namespace common;

namespace render_vk
{
//  ----------------------------------------------------------------------------
// void create_uniform_buffers(
//     VkPhysicalDevice physical_device,
//     VkDevice device,
//     const VulkanSwapchain& swapchain,
//     std::vector<VkBuffer>& uniform_buffers,
//     std::vector<VkDeviceMemory>& uniform_buffers_memory
// ) {
//     const VkDeviceSize size = sizeof(UboDataDynamic);

//     const size_t image_count = swapchain.images.size();

//     uniform_buffers.resize(image_count);
//     uniform_buffers_memory.resize(image_count);

//     for (size_t n = 0; n < image_count; ++n) {
//         create_buffer(
//             physical_device,
//             device,
//             size,
//             VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
//             VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
//             uniform_buffers[n],
//             uniform_buffers_memory[n]
//         );
//     }
// }

//  ----------------------------------------------------------------------------
void create_descriptor_set_layout(
    VkDevice device,
    VkDescriptorSetLayout& layout
) {
    VkDescriptorSetLayoutBinding ubo_layout_binding{};
    ubo_layout_binding.binding = 0;
    ubo_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;//VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    ubo_layout_binding.descriptorCount = 1;
    ubo_layout_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    ubo_layout_binding.pImmutableSamplers = nullptr;

    VkDescriptorSetLayoutBinding sampler_layout_binding{};
    sampler_layout_binding.binding = 1;
    sampler_layout_binding.descriptorCount = 1;
    sampler_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    sampler_layout_binding.pImmutableSamplers = nullptr;
    sampler_layout_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    std::array<VkDescriptorSetLayoutBinding, 2> bindings = {
        ubo_layout_binding,
        sampler_layout_binding,
    };

    VkDescriptorSetLayoutCreateInfo layout_info{};
    layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layout_info.bindingCount = static_cast<uint32_t>(bindings.size());
    layout_info.pBindings = bindings.data();

    if (vkCreateDescriptorSetLayout(device, &layout_info, nullptr, &layout) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create descriptor set layout.");
    }
}
}
