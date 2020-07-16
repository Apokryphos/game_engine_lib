#include "common/log.hpp"
#include "render_vk/buffer.hpp"
#include "render_vk/descriptor_set_layout.hpp"
#include "render_vk/vulkan.hpp"
#include "render_vk/vulkan_swapchain.hpp"
#include <array>
#include <stdexcept>

using namespace common;

namespace render_vk
{
//  ----------------------------------------------------------------------------
void create_descriptor_set_layout(
    VkDevice device,
    VkDescriptorSetLayout& layout
) {
    VkDescriptorSetLayoutBinding frame_ubo_layout_binding{};
    frame_ubo_layout_binding.binding = 0;
    frame_ubo_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    frame_ubo_layout_binding.descriptorCount = 1;
    frame_ubo_layout_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    frame_ubo_layout_binding.pImmutableSamplers = nullptr;

    VkDescriptorSetLayoutBinding object_ubo_layout_binding{};
    object_ubo_layout_binding.binding = 1;
    object_ubo_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    object_ubo_layout_binding.descriptorCount = 1;
    object_ubo_layout_binding.pImmutableSamplers = nullptr;
    object_ubo_layout_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    VkDescriptorSetLayoutBinding sampler_layout_binding{};
    sampler_layout_binding.binding = 2;
    sampler_layout_binding.descriptorCount = 1;
    sampler_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    sampler_layout_binding.pImmutableSamplers = nullptr;
    sampler_layout_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    std::array<VkDescriptorSetLayoutBinding, 3> bindings = {
        frame_ubo_layout_binding,
        object_ubo_layout_binding,
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
