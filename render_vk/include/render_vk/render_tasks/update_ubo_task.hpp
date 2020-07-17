#pragma once

#include "render_vk/dynamic_uniform_buffer.hpp"
#include "render_vk/render_job_manager.hpp"
#include "render_vk/ubo.hpp"
#include "render_vk/vulkan.hpp"
#include <glm/vec3.hpp>
#include <cstdint>
#include <string>

namespace render_vk
{
struct UpdateUboArgs
{
    //  The frame number. Used when posting finished frames.
    uint32_t frame;

    //  Swapchain image number.
    uint32_t swapchain_image;

    //  Build these vectors by iterating over drawable entities and
    //  fetching the component values. They will be in the correct order.
    std::vector<uint32_t> texture_ids;
    std::vector<glm::vec3> positions;

    DynamicUniformBuffer<ObjectUbo>* object_uniform;
};

RenderJobResult task_update_ubo(RenderThreadState& state, UpdateUboArgs& args);
}
