#include "assets/asset_manager.hpp"
#include "assets/texture_create_args.hpp"
#include "common/log.hpp"
#include "render_vk/mesh.hpp"
#include "render_vk/spine.hpp"
#include "render_vk/spine_model.hpp"
#include "render_vk/texture.hpp"
#include "render_vk/texture_id.hpp"
#include <glm/mat4x4.hpp>
#include <spine/spine.h>
#include <future>

using namespace assets;
using namespace common;
using namespace spine;

namespace render_vk
{
//  spine-cpp appears to only need a texture to set the width and height of
//  atlas pages.
class SpineTextureLoader : public spine::TextureLoader
{
    TextureAsset m_texture_asset;
public:
    SpineTextureLoader(TextureAsset texture_asset)
    : m_texture_asset(texture_asset) {
    }

    // virtual ~SpineTextureLoader() {}

    virtual void load(spine::AtlasPage& page, const spine::String& path) override {
        page.setRendererObject(this);
        page.width = m_texture_asset.width;
        page.height = m_texture_asset.height;
    }

    //  Texture unloading is handled by TextureManager
    virtual void unload(void* texture) override {}
};

//  ----------------------------------------------------------------------------
static void populate_mesh(
    spine::SkeletonData* skeleton_data,
    spine::Skeleton* skeleton,
    std::vector<Mesh>& meshes,
    std::vector<AttachmentInfo>& attachment_infos
) {
    assert(skeleton_data != nullptr);
    assert(skeleton != nullptr);

    auto& skins = skeleton_data->getSkins();
    const auto skin_count = skins.size();

    auto& slots = skeleton->getSlots();
    const auto slot_count = slots.size();

    for (size_t n = 0; n < skin_count; ++n) {
        AttachmentInfo attachment_info {};
        for (int s = 0; s < slot_count; ++s) {
            Mesh mesh {};

            Skin* skin = skins[n];
            attachment_info.skin = n;

            Slot* slot = slots[s];
            attachment_info.slot = s;

            Attachment* attachment = slot->getAttachment();
            if (attachment == nullptr) {
                continue;
            }

            // Texture* texture = NULL;
            unsigned short* indices = NULL;

            Color skeleton_color = skeleton->getColor();
            Color slot_solor = slot->getColor();
            Color tint(
                skeleton_color.r * slot_solor.r,
                skeleton_color.g * slot_solor.g,
                skeleton_color.b * slot_solor.b,
                skeleton_color.a * slot_solor.a
            );

            Vector<float> vertices;

            if (attachment->getRTTI().isExactly(RegionAttachment::rtti)) {
                RegionAttachment* regionAttachment = (RegionAttachment*)attachment;

                // Our engine specific Texture is stored in the AtlasRegion which was
                // assigned to the attachment on load. It represents the texture atlas
                // page that contains the image the region attachment is mapped to.
                // texture = (Texture*)((AtlasRegion*)regionAttachment->getRendererObject())->page->getRendererObject();

                // Ensure there is enough room for vertices
                vertices.setSize(8, 0);

                // Computed the world vertices positions for the 4 vertices that make up
                // the rectangular region attachment. This assumes the world transform of the
                // bone to which the slot (and hence attachment) is attached has been calculated
                // before rendering via Skeleton::updateWorldTransform(). The vertex positions
                // will be written directly into the vertices array, with a stride of sizeof(Vertex)
                regionAttachment->computeWorldVertices(
                    slot->getBone(),
                    &vertices.buffer()[0],
                    0,
                    2
                );

                Vector<float>& uv = regionAttachment->getUVs();

                // copy color and UVs to the vertices
                mesh.vertices.resize(4);
                for (size_t j = 0, l = 0; j < 4; j++, l+=2) {
                    mesh.vertices[j].position = glm::vec3(vertices[l+0], vertices[l+1], 0.0f);
                    mesh.vertices[j].color = glm::vec3(tint.r, tint.g, tint.b);
                    mesh.vertices[j].tex_coord = glm::vec2(uv[l+0], uv[l+1]);
                }

                //  Counter-clockwise order
                //  set the indices, 2 triangles forming a quad
                static unsigned short quadIndices[] = {0, 1, 2, 2, 3, 0};
                mesh.indices.resize(6);
                for (int n = 0; n < 6; ++n) {
                    mesh.indices[n] = quadIndices[n];
                }
            } else if (attachment->getRTTI().isExactly(MeshAttachment::rtti)) {
                MeshAttachment* meshAttachment = (MeshAttachment*)attachment;

                // Vector<float>& vertices = meshAttachment->getVertices();
                vertices.setSize(meshAttachment->getVertices().size(), 0);

                meshAttachment->computeWorldVertices(*slot, vertices);
                const auto vertex_count = vertices.size() / 2;

                meshAttachment->updateUVs();
                Vector<float>& uv = meshAttachment->getUVs();
                const auto uv_count = uv.size() / 2;

                if (vertex_count != uv_count) {
                    //  Mesh has weighted vertices
                    //  Vertex buffer contains more data than Vector2 floats
                    continue;
                }

                // copy color and UVs to the vertices
                mesh.vertices.resize(vertex_count);
                for (size_t j = 0, l = 0; j < vertex_count; j++, l+=2) {
                    mesh.vertices[j].position = glm::vec3(vertices[l+0], vertices[l+1], 0.0f);
                    mesh.vertices[j].color = glm::vec3(tint.r, tint.g, tint.b);
                    mesh.vertices[j].tex_coord = glm::vec2(uv[l+0], uv[l+1]);
                }

                //  Counter-clockwise order
                auto& triangles = meshAttachment->getTriangles();
                mesh.indices.resize(triangles.size());
                for (size_t t = 0; t < triangles.size(); ++t) {
                    mesh.indices[t] = triangles[t];
                }
            } else {
                //  Skip adding attachment info
                continue;
            }

            attachment_infos.push_back(attachment_info);
            ++attachment_info.index;

            meshes.push_back(mesh);
        }
    }
}

//  ----------------------------------------------------------------------------
std::unique_ptr<SpineModel> load_spine(
    const std::string& path,
    TextureAsset& texture_asset
) {
    //  Create a new texture loader
    auto texture_loader = std::make_unique<SpineTextureLoader>(texture_asset);

    //  Load atlas
    const spine::String atlas_path = (path + ".atlas").c_str();
    auto atlas = std::make_unique<Atlas>(atlas_path, texture_loader.get());

    //  Load skeleton JSON
    SkeletonJson json(atlas.get());
    json.setScale(2);

    //  Load skeleton data
    const spine::String skeleton_path = (path + ".json").c_str();
    SkeletonData* skeleton_data = json.readSkeletonDataFile(skeleton_path);

    if (!skeleton_data) {
        log_error(
            "Failed to load Spine skeleton data '%s'.",
            skeleton_path.buffer()
        );
        log_error(json.getError().buffer());
        throw std::runtime_error("Failed to load Spine skeleton data.");
    }

    //  Create animation state data
    auto anim_state_data = std::make_unique<AnimationStateData>(skeleton_data);

    //  Create skeleton
    auto skeleton = std::make_unique<Skeleton>(skeleton_data);

    auto spine_model = std::make_unique<SpineModel>();
    spine_model->atlas = std::move(atlas);
    spine_model->anim_state_data = std::move(anim_state_data);
    spine_model->skeleton_data = skeleton_data;
    spine_model->skeleton = std::move(skeleton);
    spine_model->texture_loader = std::move(texture_loader);

    //  Estimate mesh vertex and index count
    populate_mesh(
        spine_model->skeleton_data,
        spine_model->skeleton.get(),
        spine_model->meshes,
        spine_model->attachment_infos
    );

    log_debug(
        "Loaded Spine model '%s' (%d).",
        path.c_str(),
        spine_model->model.get_id()
    );

    return spine_model;
}
}
