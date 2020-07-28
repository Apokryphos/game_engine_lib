#include "common/log.hpp"
#include "render_vk/gltf.hpp"
#include "render_vk/index_buffer.hpp"
#include "render_vk/vertex_buffer.hpp"
#include "render_vk/mesh.hpp"
#include "tiny_obj_loader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <filesystem>
#include <stdexcept>

namespace fs = std::filesystem;

using namespace common;
using namespace tinygltf;

namespace render_vk
{
// -----------------------------------------------------------------------------
template <typename T>
inline void add_indices(
   const tinygltf::Buffer& buffer,
   const tinygltf::BufferView& view,
   Mesh& mesh
) {
   const uint32_t start_index =
      mesh.indices.empty() ?
      0 :
      *std::max_element(mesh.indices.begin(), mesh.indices.end()) + 1;

   const T* data = reinterpret_cast<const T*>(&buffer.data[view.byteOffset]);
   const auto index_count = view.byteLength / sizeof(T);

   for (size_t n = 0; n < index_count; ++n) {
      mesh.indices.push_back(start_index + data[n]);
   }
}

// -----------------------------------------------------------------------------
static void load_glb(Mesh& mesh, const std::string& path) {
   Model model;
   TinyGLTF loader;
   std::string err;
   std::string warn;

   const bool ret = loader.LoadBinaryFromFile(&model, &err, &warn, path);

   if (!warn.empty()) {
      log_error(warn.c_str());
   }

   if (!err.empty()) {
      throw std::runtime_error(err);
   }

   if (!ret) {
      throw std::runtime_error("Failed to parse glTF.");
   }

   for (auto& node : model.nodes) {
      if (node.mesh < 0) {
         continue;
      }

      auto m = model.meshes[node.mesh];

      // Convert node translation matrix
      glm::mat4 translate(1.0f);
      if (node.translation.size() == 3) {
         translate[0][3] = node.translation[0];
         translate[1][3] = node.translation[1];
         translate[2][3] = node.translation[2];
      }

      log_debug("Processing mesh '%s'...", m.name.c_str());

      // Load vertex attributes
      int pos_buffer_index = -1;
      int index_buffer_index = -1;
      int uv_buffer_index = -1;
      int index_type;

      Accessor* pos_accessor = nullptr;
      Accessor* uv_accessor = nullptr;

      for (auto& p : m.primitives) {
         for (auto& attrib_pair : p.attributes) {
            if (attrib_pair.first == "POSITION") {
               Accessor& accessor = model.accessors.at(attrib_pair.second);
               pos_accessor = &accessor;

               pos_buffer_index = accessor.bufferView;
               assert(accessor.type == TINYGLTF_TYPE_VEC3);
               assert(accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT);
               assert(p.mode == TINYGLTF_MODE_TRIANGLES);

               const Accessor& index_accessor = model.accessors.at(p.indices);
               index_buffer_index = index_accessor.bufferView;
               index_type = index_accessor.componentType;
               assert(index_accessor.type == TINYGLTF_TYPE_SCALAR);
            } else if (attrib_pair.first == "TEXCOORD_0") {
               Accessor& accessor = model.accessors.at(attrib_pair.second);
               uv_accessor = &accessor;

               uv_buffer_index = accessor.bufferView;
               assert(accessor.type == TINYGLTF_TYPE_VEC2);
               assert(accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT);
               assert(p.mode == TINYGLTF_MODE_TRIANGLES);
            }
         }
      }

      if (
         pos_buffer_index < 0 ||
         uv_buffer_index < 0 ||
         index_buffer_index < 0
      ) {
         throw std::runtime_error("Failed to parse glTF mesh primitive attributes.");
      }

      // Position data
      auto& pos_buffer_view = model.bufferViews[pos_buffer_index];
      auto& pos_buffer = model.buffers[pos_buffer_view.buffer];
      float* pos_data = reinterpret_cast<float*>(&pos_buffer.data[pos_buffer_view.byteOffset]);

      assert(pos_accessor->ByteStride(pos_buffer_view) == 12);

      // UV data
      auto& uv_buffer_view = model.bufferViews[uv_buffer_index];
      auto& uv_buffer = model.buffers[uv_buffer_view.buffer];
      float* uv_data = reinterpret_cast<float*>(&uv_buffer.data[uv_buffer_view.byteOffset]);

      assert(uv_accessor->ByteStride(uv_buffer_view) == 8);

      // Number of vertices
      const auto vertex_count = (pos_buffer_view.byteLength / sizeof(float)) / 3;

      for (size_t n = 0; n < vertex_count; ++n) {
         Vertex vertex{};

         // Transform vertex using node matrices
         const glm::vec4 pos = glm::vec4(
            pos_data[n * 3 + 0],
            pos_data[n * 3 + 1],
            pos_data[n * 3 + 2],
            1.0f
         ) * translate;

         vertex.position = { pos.x, pos.y, pos.z };

         vertex.tex_coord = {
            uv_data[n * 2 + 0],
            uv_data[n * 2 + 1],
         };

         vertex.color = {1.0f, 1.0f, 1.0f};

         mesh.vertices.push_back(vertex);
      }

      // Index data
      const auto& index_buffer_view = model.bufferViews[index_buffer_index];
      const auto& index_buffer = model.buffers[index_buffer_view.buffer];
      if (index_type == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) {
         add_indices<uint16_t>(index_buffer, index_buffer_view, mesh);
      } else if (index_type == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT) {
         add_indices<uint32_t>(index_buffer, index_buffer_view, mesh);
      } else {
         throw std::runtime_error("Not implemented.");
      }
   }
}

// -----------------------------------------------------------------------------
static void load_obj(Mesh& mesh, const std::string& path) {
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
}

// -----------------------------------------------------------------------------
void load_mesh(Mesh& mesh, const std::string& path) {
    const std::string extension = fs::path(path).extension();
    if (extension == ".glb") {
       load_glb(mesh, path);
    } else if (extension == ".obj") {
       load_obj(mesh, path);
    } else {
        throw std::runtime_error("Not supported.");
    }
}
}
