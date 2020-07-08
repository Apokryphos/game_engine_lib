#include "common/log.hpp"
#include "engine/asset_manager.hpp"
#include "render/renderer.hpp"
#include <algorithm>

using namespace common;
using namespace render;

namespace engine
{
//  ----------------------------------------------------------------------------
AssetManager::AssetManager()
: m_unique_id(1) {
}

//  ----------------------------------------------------------------------------
AssetManager::~AssetManager() {
    shutdown();
}

//  ----------------------------------------------------------------------------
AssetId AssetManager::get_unique_id() {
    return m_unique_id++;
}

//  ----------------------------------------------------------------------------
AssetId AssetManager::load_model(
    Renderer& renderer,
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

    const AssetId id = get_unique_id();

    renderer.load_model(id, path);

    Entry entry{};
    entry.id = id;
    entry.path = path;
    m_models.push_back(entry);

    log_debug("Loaded model '%s' (%d).", path.c_str(), id);

    return id;
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
