#include "render_vk/index_buffer.hpp"
#include "render_vk/vertex_buffer.hpp"
#include "render_vk/mesh.hpp"
#include "tiny_obj_loader.h"
#include <stdexcept>

namespace render_vk
{
// -----------------------------------------------------------------------------
void load_mesh(Mesh& mesh, const std::string& path) {
   tinyobj::attrib_t attrib;
   std::vector<tinyobj::shape_t> shapes;
   std::vector<tinyobj::material_t> materials;
   std::string warn, err;
   if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.c_str())) {
      throw std::runtime_error(warn + err);
   }

   for (const auto& shape : shapes) {
      for (const auto& index : shape.mesh.indices) {
         Vertex vertex{};

         vertex.position = {
            attrib.vertices[3 * index.vertex_index + 0],
            attrib.vertices[3 * index.vertex_index + 1],
            attrib.vertices[3 * index.vertex_index + 2]
         };

         // Flip vertical component
         vertex.tex_coord = {
            attrib.texcoords[2 * index.texcoord_index + 0],
            1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
         };

         vertex.color = {1.0f, 1.0f, 1.0f};

         mesh.vertices.push_back(vertex);
         mesh.indices.push_back(mesh.indices.size());
      }
   }

   get_vertices() = mesh.vertices;
   get_indices() = mesh.indices;
}
}
