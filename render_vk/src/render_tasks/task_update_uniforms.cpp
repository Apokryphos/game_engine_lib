#include "common/log.hpp"
#include "render/model_batch.hpp"
#include "render_vk/debug_utils.hpp"
#include "render_vk/dynamic_uniform_buffer.hpp"
#include "render_vk/ubo.hpp"
#include "render_vk/uniform_buffer.hpp"
#include <glm/gtc/matrix_transform.hpp>

using namespace render;

namespace render_vk
{
//  ----------------------------------------------------------------------------
void task_update_frame_uniforms(
    const glm::mat4 view,
    const glm::mat4 proj,
    UniformBuffer<FrameUbo>& frame_uniform
) {
    //  Update frame UBO
    FrameUbo frame_ubo{};
    frame_ubo.view = view;
    frame_ubo.proj = proj;

    //  GLM (OpenGL) uses inverted Y clip coordinate
    frame_ubo.proj[1][1] *= -1;

    //  Copy frame UBO struct to uniform buffer
    frame_uniform.copy(frame_ubo);
}

//  ----------------------------------------------------------------------------
void task_update_object_uniforms(
    const std::vector<ModelBatch>& batches,
    DynamicUniformBuffer<ObjectUbo>& object_uniform
) {
    //   Build vectors for uniform buffers
    std::vector<glm::vec3> positions;
    std::vector<uint32_t> texture_ids;
    for (const ModelBatch& batch : batches) {
        positions.insert(
            positions.end(),
            batch.positions.begin(),
            batch.positions.end()
        );

        texture_ids = std::vector<uint32_t>(positions.size(), batch.texture_id);
    }

    assert(positions.size() == texture_ids.size());

    const size_t object_count = positions.size();
    assert(object_count > 0);

    std::vector<ObjectUbo> data(object_count);
    for (size_t n = 0; n < object_count; ++n)  {
        data[n].texture_index = texture_ids[n];
        data[n].model = glm::translate(glm::mat4(1.0f), positions[n]);
    }

    //  Copy object UBO structs to dynamic uniform buffer
    object_uniform.copy(data);
}
}