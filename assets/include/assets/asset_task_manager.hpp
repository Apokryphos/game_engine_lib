#pragma once

#include "assets/asset_id.hpp"
#include "assets/spine_load_args.hpp"
#include "assets/texture_asset.hpp"
#include "assets/texture_load_args.hpp"
#include "render/texture_create_args.hpp"
#include <future>
#include <memory>
#include <mutex>
#include <optional>
#include <queue>
#include <thread>
#include <vector>

namespace assets
{
class AssetTaskManager
{
public:
    template <typename T>
    using JobPromise = std::optional<std::promise<T>>;

public:
    AssetTaskManager();
    virtual ~AssetTaskManager();
    AssetTaskManager(const AssetTaskManager&) = delete;
    AssetTaskManager& operator=(const AssetTaskManager&) = delete;
    //  Enqueues a load model job for worker threads to complete
    virtual void load_model(uint32_t id, const std::string& path) = 0;
    //  Enqueues a Spine skeleton for worker threads to complete
    virtual void load_spine(
        AssetId id,
        SpineLoadArgs& load_args,
        const render::TextureCreateArgs& args
    ) = 0;
    //  Enqueues a load texture job for worker threads to complete
    virtual void load_texture(
        AssetId id,
        TextureLoadArgs& load_args,
        const render::TextureCreateArgs& args
    ) = 0;
};
}
