#pragma once

#include "render/model_batch.hpp"
#include "render_vk/dynamic_uniform_buffer.hpp"
#include "render_vk/ubo.hpp"
#include "render_vk/uniform_buffer.hpp"
#include <glm/mat4x4.hpp>

namespace render_vk
{
void task_update_frame_uniforms(
    const glm::mat4 view,
    const glm::mat4 proj,
    UniformBuffer<FrameUbo>& frame_uniform
);

void task_update_object_uniforms(
    const std::vector<render::ModelBatch>& batches,
    DynamicUniformBuffer<ObjectUbo>& object_uniform
);
}
