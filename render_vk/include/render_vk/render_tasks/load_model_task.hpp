#pragma once

#include "common/asset.hpp"
#include "render_vk/render_job_manager.hpp"
#include "render_vk/vulkan.hpp"
#include <string>

namespace render_vk
{
struct LoadModelArgs
{
    common::AssetId id;
    std::string path;
};

RenderJobResult task_load_model(RenderThreadState& state, LoadModelArgs& args);
}
