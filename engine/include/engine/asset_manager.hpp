#pragma once

#include "common/asset.hpp"
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

    AssetId m_unique_id;
    std::vector<Entry> m_models;

    AssetId get_unique_id();

public:
    AssetManager();
    ~AssetManager();
    AssetManager(const AssetManager&) = delete;
    AssetManager& operator=(const AssetManager&) = delete;
    AssetId load_model(Renderer& renderer, const std::string& path);
    void unload_model(const AssetId id);
    void unload_models();
    void shutdown();
};
}
