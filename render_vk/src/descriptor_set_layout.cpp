#include "common/log.hpp"
#include "render_vk/descriptor_set_layout.hpp"
#include "render_vk/vulkan.hpp"
#include <array>
#include <stdexcept>

using namespace common;

namespace render_vk
{
//  ----------------------------------------------------------------------------
void create_spine_descriptor_set_layout(
    VkDevice device,
    VkDescriptorSetLayout& layout
) {
    VkDescriptorSetLayoutBinding layout_binding{};
    layout_binding.binding = 0;
    layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    layout_binding.descriptorCount = 1;
    layout_binding.pImmutableSamplers = nullptr;
    layout_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;

    std::array<VkDescriptorSetLayoutBinding, 1> bindings = {
        layout_binding,
    };

    VkDescriptorSetLayoutCreateInfo layout_info{};
    layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layout_info.bindingCount = static_cast<uint32_t>(bindings.size());
    layout_info.pBindings = bindings.data();

    if (vkCreateDescriptorSetLayout(device, &layout_info, nullptr, &layout) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create descriptor set layout.");
    }
}

//  ----------------------------------------------------------------------------
void create_glyph_descriptor_set_layout(
    VkDevice device,
    VkDescriptorSetLayout& layout
) {
    VkDescriptorSetLayoutBinding object_ubo_layout_binding{};
    object_ubo_layout_binding.binding = 0;
    object_ubo_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    object_ubo_layout_binding.descriptorCount = 1;
    object_ubo_layout_binding.pImmutableSamplers = nullptr;
    object_ubo_layout_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;

    std::array<VkDescriptorSetLayoutBinding, 1> bindings = {
        object_ubo_layout_binding,
    };

    VkDescriptorSetLayoutCreateInfo layout_info{};
    layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layout_info.bindingCount = static_cast<uint32_t>(bindings.size());
    layout_info.pBindings = bindings.data();

    if (vkCreateDescriptorSetLayout(device, &layout_info, nullptr, &layout) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create descriptor set layout.");
    }
}

//  ----------------------------------------------------------------------------
void create_object_descriptor_set_layout(
    VkDevice device,
    VkDescriptorSetLayout& layout
) {
    VkDescriptorSetLayoutBinding object_ubo_layout_binding{};
    object_ubo_layout_binding.binding = 0;
    object_ubo_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    object_ubo_layout_binding.descriptorCount = 1;
    object_ubo_layout_binding.pImmutableSamplers = nullptr;
    object_ubo_layout_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;

    VkDescriptorSetLayoutBinding sampler_layout_binding{};
    sampler_layout_binding.binding = 1;
    sampler_layout_binding.descriptorCount = 3;
    sampler_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    sampler_layout_binding.pImmutableSamplers = nullptr;
    sampler_layout_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    std::array<VkDescriptorSetLayoutBinding, 2> bindings = {
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

//  ----------------------------------------------------------------------------
void make_frame_descriptor_set_layout_binding(
    const uint32_t binding,
    VkDescriptorSetLayoutBinding& layout_binding
) {
    layout_binding.binding = binding;
    layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    layout_binding.descriptorCount = 1;
    layout_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    layout_binding.pImmutableSamplers = nullptr;
}

//  ----------------------------------------------------------------------------
void make_sampler_descriptor_set_layout_binding(
    const uint32_t binding,
    const uint32_t sampler_count,
    VkDescriptorSetLayoutBinding& layout_binding
) {
    layout_binding.binding = binding;
    layout_binding.descriptorCount = sampler_count;
    layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    layout_binding.pImmutableSamplers = nullptr;
    layout_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
}

//  ----------------------------------------------------------------------------
void create_descriptor_set_layout(
    const VkDevice device,
    const std::vector<VkDescriptorSetLayoutBinding>& bindings,
    VkDescriptorSetLayout& layout
) {
    VkDescriptorSetLayoutCreateInfo layout_info{};
    layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layout_info.bindingCount = static_cast<uint32_t>(bindings.size());
    layout_info.pBindings = bindings.data();

    if (vkCreateDescriptorSetLayout(device, &layout_info, nullptr, &layout) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create descriptor set layout.");
    }
}

//  ----------------------------------------------------------------------------
void create_frame_descriptor_set_layout(
    VkDevice device,
    VkDescriptorSetLayout& layout
) {
    VkDescriptorSetLayoutBinding frame_ubo_layout_binding{};
    make_frame_descriptor_set_layout_binding(0, frame_ubo_layout_binding);
    create_descriptor_set_layout(device, { frame_ubo_layout_binding }, layout);
}

//  ----------------------------------------------------------------------------
void create_sampler_descriptor_set_layout(
    const VkDevice device,
    const uint32_t sampler_count,
    VkDescriptorSetLayout& layout
) {
    VkDescriptorSetLayoutBinding sampler_layout_binding{};
    make_sampler_descriptor_set_layout_binding(
        0,
        sampler_count,
        sampler_layout_binding
    );

    VkDescriptorBindingFlagsEXT bind_flag = VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT_EXT;

    VkDescriptorSetLayoutBindingFlagsCreateInfoEXT extended_info{};
    extended_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO_EXT;
    extended_info.pNext = nullptr;
    extended_info.bindingCount = 1u;
    extended_info.pBindingFlags = &bind_flag;

    std::array<VkDescriptorSetLayoutBinding, 1> bindings {
        sampler_layout_binding
    };

    VkDescriptorSetLayoutCreateInfo layout_info{};
    layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layout_info.bindingCount = static_cast<uint32_t>(bindings.size());
    layout_info.pBindings = bindings.data();
    layout_info.pNext = &extended_info;

    if (vkCreateDescriptorSetLayout(device, &layout_info, nullptr, &layout) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create descriptor set layout.");
    }
}

//  ----------------------------------------------------------------------------
void create_descriptor_set_layouts(
    const VkDevice device,
    const uint32_t sampler_count,
    DescriptorSetLayouts& layouts
) {
    create_frame_descriptor_set_layout(device, layouts.frame);
    // create_object_descriptor_set_layout(device, layouts.object);
    create_glyph_descriptor_set_layout(device, layouts.glyph);
    create_spine_descriptor_set_layout(device, layouts.spine);
    create_sampler_descriptor_set_layout(device, sampler_count, layouts.texture_sampler);
}
}
