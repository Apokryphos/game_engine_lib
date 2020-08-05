#include "assets/asset_manager.hpp"
#include "assets/texture_create_args.hpp"
#include "common/log.hpp"
#include "render_vk/mesh.hpp"
#include "render_vk/spine.hpp"
#include "render_vk/spine_model.hpp"
#include "render_vk/texture.hpp"
#include "render_vk/texture_id.hpp"
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
static void reserve_mesh(Mesh& mesh, SkeletonData& skeleton_data) {
    const Vector<SlotData*>& slots = skeleton_data.getSlots();
    const auto slot_count = slots.size();

    Vector<Skin*>& skins = skeleton_data.getSkins();
    const auto skin_count = skins.size();

    uint64_t vertex_count = 0;
    uint64_t index_count = 0;

    for (uint32_t skin_index = 0; skin_index < skin_count; ++skin_index) {
        for (uint32_t slot_index = 0; slot_index < slot_count; ++slot_index) {
            Vector<Attachment*> attachments;
            skins[skin_index]->findAttachmentsForSlot(slot_index, attachments);

            const auto attachment_count = attachments.size();
            for (auto attachment_index = 0; attachment_index < attachment_count; ++ attachment_index) {
                Attachment* attachment = attachments[attachment_index];
                if (attachment == nullptr) {
                    continue;
                }

                if (attachment->getRTTI().isExactly(RegionAttachment::rtti)) {
                    vertex_count += 4;
                    index_count += 6;
                } else if (attachment->getRTTI().isExactly(MeshAttachment::rtti)) {
                    MeshAttachment* mesh_attach = static_cast<MeshAttachment*>(attachment);
                    vertex_count += mesh_attach->getWorldVerticesLength() / 2;
                    index_count += mesh_attach->getTriangles().size();
                }
            }
        }
    }

    mesh.vertices.resize(vertex_count);
    mesh.indices.resize(index_count);
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
    reserve_mesh(spine_model->mesh, *skeleton_data);

    log_debug(
        "Loaded Spine model '%s' (%d).",
        path.c_str(),
        spine_model->model.get_id()
    );

    return spine_model;
}
}
