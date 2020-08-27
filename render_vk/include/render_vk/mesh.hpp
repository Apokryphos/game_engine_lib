#pragma once

#include "render_vk/glyph_vertex.hpp"
#include "render_vk/vertex.hpp"
#include "render_vk/vulkan.hpp"
#include <string>
#include <vector>

namespace render_vk
{
template <typename T>
struct MeshBase
{
   std::vector<T> vertices;
   std::vector<uint32_t> indices;
};

struct Mesh : MeshBase<Vertex> {};
struct GlyphMesh : MeshBase<GlyphVertex> {};

void load_mesh(Mesh& mesh, const std::string& path);
}
