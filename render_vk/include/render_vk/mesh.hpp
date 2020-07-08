#pragma once

#include "render_vk/vertex.hpp"
#include "render_vk/vulkan.hpp"
#include <string>
#include <vector>

namespace render_vk
{
struct Mesh
{
   std::vector<Vertex> vertices;
   std::vector<uint32_t> indices;
};

void load_mesh(Mesh& mesh, const std::string& path);
}
