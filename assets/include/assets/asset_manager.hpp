#pragma once

#include "assets/asset_id.hpp"
#include "assets/spine_load_args.hpp"
#include "assets/texture_load_args.hpp"
#include "render/texture_create_args.hpp"
#include <memory>
#include <string>
#include <vector>

namespace assets
{
class AssetTaskManager;
class SpineManager;

class AssetManager
{
public:
    struct Entry
    {
        AssetId id;
        std::string path;
    };

    AssetId m_unique_model_id   {0};
    AssetId m_unique_spine_id   {0};
    AssetId m_unique_texture_id {0};

    std::vector<Entry> m_models;
    std::vector<Entry> m_spines;
    std::vector<Entry> m_textures;

    std::shared_ptr<AssetTaskManager> m_asset_task_mgr;
    std::shared_ptr<SpineManager> m_spine_mgr;

    AssetId get_unique_model_id();
    AssetId get_unique_spine_id();
    AssetId get_unique_texture_id();

    std::vector<Entry>::const_iterator find_texture(const TextureLoadArgs& args) const;

public:
    AssetManager(
        std::shared_ptr<AssetTaskManager> asset_task_mgr,
        std::shared_ptr<SpineManager> spine_mgr
    );
    ~AssetManager();
    AssetManager(const AssetManager&) = delete;
    AssetManager& operator=(const AssetManager&) = delete;
    SpineManager& get_spine_manager();
    AssetId load_model(const std::string& path);
    AssetId load_spine(
        SpineLoadArgs& load_args,
        const render::TextureCreateArgs args = {}
    );
    AssetId load_spine(
        const std::string& path,
        const render::TextureCreateArgs args = {}
    );
    AssetId load_texture(
        TextureLoadArgs& load_args,
        const render::TextureCreateArgs args = {}
    );
    AssetId load_texture(
        const std::string& path,
        const render::TextureCreateArgs args = {}
    );
    void unload_model(const AssetId id);
    void unload_models();
    void shutdown();
};
}
