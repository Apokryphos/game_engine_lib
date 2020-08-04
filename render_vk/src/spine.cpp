#include "assets/asset_manager.hpp"
#include "common/log.hpp"
#include "render/texture_create_args.hpp"
#include "render_vk/spine.hpp"
#include "render_vk/texture.hpp"
#include "render_vk/texture_id.hpp"
#include <spine/spine.h>
#include <future>

using namespace assets;
using namespace common;
using namespace render;
using namespace spine;

namespace render_vk
{
//  spine-cpp appears to only need a texture to set the width and height of
//  atlas pages.
class SpineTextureLoader : public TextureLoader
{
    TextureAsset m_texture_asset;
public:
    SpineTextureLoader(TextureAsset texture_asset)
    : m_texture_asset(texture_asset) {
    }

    virtual ~SpineTextureLoader() {}

    virtual void load(spine::AtlasPage& page, const spine::String& path) {
        page.width = m_texture_asset.width;
        page.height = m_texture_asset.height;
    }

    //  Texture unloading is handled by TextureManager
    virtual void unload(void* texture) {}
};

//  ----------------------------------------------------------------------------
static void load_spine(const std::string& path, TextureAsset& texture_asset) {
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
}

//  ----------------------------------------------------------------------------
void load_spine(
    const TextureId texture_id,
    const std::string& path,
    AssetManager& asset_mgr
) {
    //  Load texture through asset manager
    TextureLoadArgs load_args {};
    load_args.path = path;
    load_args.promise = TextureAssetPromise();
    TextureCreateArgs create_args {};
    asset_mgr.load_texture(load_args, create_args);

    //  Wait for texture to finish loading on worker threads
    std::future future = load_args.promise.value().get_future();
    TextureAsset texture_asset = future.get();

    load_spine(path, texture_asset);
}
}
