#pragma once

#include "common/asset.hpp"
#include "render/texture_load_args.hpp"
#include <memory>
#include <vector>

namespace render
{
class Renderer;
};

namespace engine
{
class AssetManager
{
    using AssetId = common::AssetId;
    using Renderer = render::Renderer;

    struct Entry
    {
        AssetId id;
        std::string path;
    };

    AssetId m_unique_model_id {0};
    AssetId m_unique_texture_id {0};

    std::vector<Entry> m_models;
    std::vector<Entry> m_textures;

    AssetId get_unique_model_id();
    AssetId get_unique_texture_id();

public:
    AssetManager();
    ~AssetManager();
    AssetManager(const AssetManager&) = delete;
    AssetManager& operator=(const AssetManager&) = delete;
    AssetId load_model(Renderer& renderer, const std::string& path);
    AssetId load_texture(
        Renderer& renderer,
        const std::string& path,
        const render::TextureLoadArgs args = {}
    );
    void unload_model(const AssetId id);
    void unload_models();
    void shutdown();
};
}
