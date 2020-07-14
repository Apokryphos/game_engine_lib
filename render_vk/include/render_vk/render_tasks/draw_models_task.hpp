#pragma once

#include "render_vk/render_job_manager.hpp"
#include "render_vk/vulkan.hpp"
#include <glm/vec3.hpp>
#include <cstdint>
#include <string>

namespace render_vk
{
struct DrawModelsArgs
{
    //  Build these vectors by iterating over drawable entities and
    //  fetching the component values. They will be in the correct order.
    std::vector<uint32_t> model_ids;
    std::vector<glm::vec3> positions;
};

void task_draw_models(RenderThreadState& state, DrawModelsArgs& args);
}
