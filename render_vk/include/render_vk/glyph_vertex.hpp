#pragma once

#include "render_vk/vulkan.hpp"
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <array>

namespace render_vk
{
struct GlyphVertex
{
    glm::vec3 position;
    glm::vec3 fg_color;
    glm::vec3 bg_color;
    glm::vec2 tex_coord;
    uint32_t texture_id;

    static std::array<VkVertexInputAttributeDescription, 5> get_attribute_descriptions() {
        std::array<VkVertexInputAttributeDescription, 5> attrib_descs{};

        attrib_descs[0].binding = 0;
        attrib_descs[0].location = 0;
        attrib_descs[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attrib_descs[0].offset = offsetof(GlyphVertex, position);

        attrib_descs[1].binding = 0;
        attrib_descs[1].location = 1;
        attrib_descs[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attrib_descs[1].offset = offsetof(GlyphVertex, bg_color);

        attrib_descs[2].binding = 0;
        attrib_descs[2].location = 2;
        attrib_descs[2].format = VK_FORMAT_R32G32B32_SFLOAT;
        attrib_descs[2].offset = offsetof(GlyphVertex, fg_color);

        attrib_descs[3].binding = 0;
        attrib_descs[3].location = 3;
        attrib_descs[3].format = VK_FORMAT_R32G32_SFLOAT;
        attrib_descs[3].offset = offsetof(GlyphVertex, tex_coord);

        attrib_descs[4].binding = 0;
        attrib_descs[4].location = 4;
        attrib_descs[4].format = VK_FORMAT_R32_UINT;
        attrib_descs[4].offset = offsetof(GlyphVertex, texture_id);

        return attrib_descs;
    }

    static VkVertexInputBindingDescription get_binding_description() {
        VkVertexInputBindingDescription binding_desc{};
        binding_desc.binding = 0;
        binding_desc.stride = sizeof(GlyphVertex);
        binding_desc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return binding_desc;
    }
};
}
