#pragma once

#include "render_vk/vulkan.hpp"
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <array>

namespace render_vk
{
struct Vertex
{
    glm::vec3 position;
    glm::vec3 color;
    glm::vec2 tex_coord;

    static std::array<VkVertexInputAttributeDescription, 3> get_attribute_descriptions() {
        std::array<VkVertexInputAttributeDescription, 3> attrib_descs{};

        attrib_descs[0].binding = 0;
        attrib_descs[0].location = 0;
        attrib_descs[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attrib_descs[0].offset = offsetof(Vertex, position);

        attrib_descs[1].binding = 0;
        attrib_descs[1].location = 1;
        attrib_descs[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attrib_descs[1].offset = offsetof(Vertex, color);

        attrib_descs[2].binding = 0;
        attrib_descs[2].location = 2;
        attrib_descs[2].format = VK_FORMAT_R32G32_SFLOAT;
        attrib_descs[2].offset = offsetof(Vertex, tex_coord);

        return attrib_descs;
    }

    static VkVertexInputBindingDescription get_binding_description() {
        VkVertexInputBindingDescription binding_desc{};
        binding_desc.binding = 0;
        binding_desc.stride = sizeof(Vertex);
        binding_desc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return binding_desc;
    }
};
}
