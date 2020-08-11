#include "assets/asset_manager.hpp"
#include "assets/texture_create_args.hpp"
#include "common/log.hpp"
#include "render_vk/mesh.hpp"
#include "render_vk/spine.hpp"
#include "render_vk/spine_model.hpp"
#include "render_vk/texture.hpp"
#include "render_vk/texture_id.hpp"
#include <glm/mat4x4.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <spine/spine.h>
#include <algorithm>
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
template <typename T>
static void copy_vertex_position(
    T& positions,
    std::vector<Vertex>& vertices
) {
    for (size_t n = 0, m = 0; m < positions.size(); n++, m+=2) {
        vertices[n].position = glm::vec3(positions[m+0], positions[m+1], 0.0f);
    }
}

//  ----------------------------------------------------------------------------
static void copy_quad_indices(std::vector<uint32_t>& indices) {
    static unsigned short quad_indices[] = { 0, 1, 2, 2, 3, 0 };
    indices.resize(6);
    for (int n = 0; n < 6; ++n) {
        indices[n] = quad_indices[n];
    }
}

//  ----------------------------------------------------------------------------
static void copy_vertex_color(
    const Color& color,
    std::vector<Vertex>& vertices
) {
    for (size_t n = 0; n < vertices.size(); ++n) {
        vertices[n].color = glm::vec3(color.r, color.g, color.b);
    }
}

//  ----------------------------------------------------------------------------
template <typename T>
static void copy_vertex_uv(
    T* attachment,
    std::vector<Vertex>& vertices
) {
    Vector<float>& uv = attachment->getUVs();
    for (size_t n = 0, m = 0; m < uv.size(); n++, m +=2 ) {
        vertices[n].tex_coord = glm::vec2(uv[m+0], uv[m+1]);
    }
}

//  ----------------------------------------------------------------------------
static void load_mesh_attachment(
    Slot* slot,
    MeshAttachment* attachment,
    const Color& tint,
    Mesh& mesh
) {
    Vector<float> positions;
    positions.setSize(attachment->getWorldVerticesLength(), 0);
    attachment->computeWorldVertices(*slot, positions);
    const auto vertex_count = positions.size() / 2;
    mesh.vertices.resize(vertex_count);
    copy_vertex_position(positions, mesh.vertices);

    copy_vertex_color(tint, mesh.vertices);
    copy_vertex_uv(attachment, mesh.vertices);

    auto& triangles = attachment->getTriangles();
    mesh.indices.resize(triangles.size());
    for (size_t t = 0; t < triangles.size(); ++t) {
        mesh.indices[t] = triangles[t];
    }
}

//  ----------------------------------------------------------------------------
static void load_region_attachment(
    Slot* slot,
    RegionAttachment* attachment,
    const Color& tint,
    Mesh& mesh
) {
    //  Position data
    Vector<float> positions;
    positions.setSize(8, 0);

    attachment->computeWorldVertices(
        slot->getBone(),
        &positions.buffer()[0],
        0,
        2
    );

    mesh.vertices.resize(4);
    copy_vertex_position(positions, mesh.vertices);
    copy_vertex_color(tint, mesh.vertices);
    copy_vertex_uv(attachment, mesh.vertices);
    copy_quad_indices(mesh.indices);
}

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
            Skin* skin = skins[n];
            attachment_info.skin = n;

            Slot* slot = slots[s];
            attachment_info.slot = s;

            Attachment* attachment = slot->getAttachment();
            if (attachment == nullptr) {
                continue;
            }

            attachment_info.attachment_name = std::string(attachment->getName().buffer());

            Color skeleton_color = skeleton->getColor();
            Color slot_solor = slot->getColor();
            Color tint(
                skeleton_color.r * slot_solor.r,
                skeleton_color.g * slot_solor.g,
                skeleton_color.b * slot_solor.b,
                skeleton_color.a * slot_solor.a
            );

            Mesh mesh {};

            if (attachment->getRTTI().isExactly(RegionAttachment::rtti)) {
                RegionAttachment* region_attachment = (RegionAttachment*)attachment;
                load_region_attachment(slot, region_attachment, tint, mesh);
            } else if (attachment->getRTTI().isExactly(MeshAttachment::rtti)) {
                MeshAttachment* mesh_attachment = (MeshAttachment*)attachment;
                load_mesh_attachment(slot, mesh_attachment, tint, mesh);
            } else {
                //  Skip adding attachment info
                continue;
            }

            if (mesh.vertices.empty()) {
                continue;
            }

            //  Rotate vertices
            for (Vertex& vertex : mesh.vertices) {
                vertex.position = glm::rotateX(vertex.position, glm::radians(180.0f));
                vertex.position.z = 0;
            }

            //  Reverse index order
            std::reverse(mesh.indices.begin(), mesh.indices.end());

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

    //  Create animation state
    auto anim_state = std::make_unique<AnimationState>(anim_state_data.get());
    anim_state->setAnimation(0, "idle", true);
    anim_state->apply(*skeleton.get());

    skeleton->updateWorldTransform();

    auto spine_model = std::make_unique<SpineModel>();
    spine_model->atlas = std::move(atlas);
    spine_model->anim_state_data = std::move(anim_state_data);
    spine_model->anim_state = std::move(anim_state);
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
