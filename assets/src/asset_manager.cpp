#include "assets/asset_manager.hpp"
#include "assets/asset_task_manager.hpp"
#include "common/log.hpp"
#include <algorithm>

using namespace common;

namespace assets
{
//  ----------------------------------------------------------------------------
AssetManager::AssetManager(
    std::shared_ptr<AssetTaskManager> asset_task_mgr,
    std::shared_ptr<SpineManager> spine_mgr
)
: m_asset_task_mgr(std::move(asset_task_mgr)),
  m_spine_mgr(std::move(spine_mgr)) {
}

//  ----------------------------------------------------------------------------
AssetManager::~AssetManager() {
    shutdown();
}

//  ----------------------------------------------------------------------------
AssetId AssetManager::create_glyph_mesh(const GlyphMeshCreateArgs& args) {
    const AssetId id = get_unique_model_id();
    m_asset_task_mgr->create_glyph_mesh(id, args);
    return id;
}

//  ----------------------------------------------------------------------------
std::vector<AssetManager::Entry>::const_iterator AssetManager::find_texture(
    const TextureLoadArgs& args
) const {
    return std::find_if(
        m_textures.begin(),
        m_textures.end(),
        [&args](const Entry& entry) {
            return entry.path == args.path;
        }
    );
}

//  ----------------------------------------------------------------------------
SpineManager& AssetManager::get_spine_manager() {
    return *m_spine_mgr;
}

//  ----------------------------------------------------------------------------
AssetId AssetManager::get_texture_id(const std::string& path) {
    const auto find = std::find_if(
        m_textures.begin(),
        m_textures.end(),
        [&path](const Entry& entry) {
            return entry.path == path;
        }
    );

    if (find != m_textures.end()) {
        return (*find).id;
    }

    return 0;
}

//  ----------------------------------------------------------------------------
AssetId AssetManager::get_unique_model_id() {
    return m_unique_model_id++;
}

//  ----------------------------------------------------------------------------
AssetId AssetManager::get_unique_spine_id() {
    return m_unique_spine_id++;
}

//  ----------------------------------------------------------------------------
AssetId AssetManager::get_unique_texture_id() {
    return m_unique_texture_id++;
}

//  ----------------------------------------------------------------------------
AssetId AssetManager::load_model(
    const std::string& path
) {
    const auto find = std::find_if(
        m_models.begin(),
        m_models.end(),
        [&path](const Entry& entry) {
            return entry.path == path;
        }
    );

    if (find != m_models.end()) {
        return (*find).id;
    }

    const AssetId id = get_unique_model_id();

    m_asset_task_mgr->load_model(id, path);

    Entry entry{};
    entry.id = id;
    entry.path = path;
    m_models.push_back(entry);

    log_debug("Loaded model '%s' (%d).", path.c_str(), id);

    return id;
}

//  ----------------------------------------------------------------------------
AssetId AssetManager::load_spine(
    SpineLoadArgs& load_args,
    const TextureCreateArgs create_args
) {
    const std::string& path = load_args.path;

    const auto find = std::find_if(
        m_spines.begin(),
        m_spines.end(),
        [&path](const Entry& entry) {
            return entry.path == path;
        }
    );

    if (find != m_spines.end()) {
        return (*find).id;
    }

    //  Check if texture is loaded
    TextureLoadArgs texture_load_args {};
    texture_load_args.path = load_args.path + ".png";
    const auto texture_itr = find_texture(texture_load_args);
    if (texture_itr == m_textures.end()) {
        assert(!load_args.texture_future.valid());

        //  Load texture on worker thread and wait for it later
        texture_load_args.promise = make_texture_asset_promise();
        load_args.texture_future = get_texture_asset_future(texture_load_args.promise);
        load_texture(texture_load_args, create_args);
    }

    const AssetId id = get_unique_spine_id();

    m_asset_task_mgr->load_spine(id, load_args, create_args);

    Entry entry{};
    entry.id = id;
    entry.path = path;
    m_spines.push_back(entry);

    log_debug("Loaded Spine '%s' (%d).", path.c_str(), id);

    return id;
}

//  ----------------------------------------------------------------------------
AssetId AssetManager::load_spine(
    const std::string& path,
    const TextureCreateArgs args
) {
    SpineLoadArgs load_args {};
    load_args.path = path;
    return load_spine(load_args, args);
}

//  ----------------------------------------------------------------------------
AssetId AssetManager::load_texture(
    TextureLoadArgs& load_args,
    const TextureCreateArgs create_args
) {
    //  Check if texture is loaded
    const auto find = find_texture(load_args);
    if (find != m_textures.end()) {
        return (*find).id;
    }

    const AssetId id = get_unique_texture_id();

    m_asset_task_mgr->load_texture(id, load_args, create_args);

    Entry entry{};
    entry.id = id;
    entry.path = load_args.path;
    m_textures.push_back(entry);

    log_debug("Loaded texture '%s' (%d).", load_args.path.c_str(), id);

    return id;
}

//  ----------------------------------------------------------------------------
AssetId AssetManager::load_texture(
    const std::string& path,
    const TextureCreateArgs args
) {
    TextureLoadArgs load_args {};
    load_args.path = path;
    return load_texture(load_args, args);
}

//  ----------------------------------------------------------------------------
void AssetManager::unload_model(const AssetId id) {
    auto find = std::find_if(
        m_models.begin(),
        m_models.end(),
        [id](const auto& entry) {
            return entry.id == id;
        }
    );

    if (find != m_models.end()) {
        auto& model = *find;
        // model->unload();
        // log_debug("Unloaded model '%s' (%d).", model->get_path().c_str(), id);
        m_models.erase(find);
    }
}

//  ----------------------------------------------------------------------------
void AssetManager::unload_models() {
    // for (auto& model : m_models) {
    //     model->unload();
    // }
    m_models.clear();
}

//  ----------------------------------------------------------------------------
void AssetManager::shutdown() {
    unload_models();
}
}
