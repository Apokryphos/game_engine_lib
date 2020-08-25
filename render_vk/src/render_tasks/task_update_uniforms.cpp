#include "common/log.hpp"
#include "render/glyph_batch.hpp"
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
    FrameUbo frame_ubo,
    UniformBuffer<FrameUbo>& frame_uniform
) {
    //  GLM (OpenGL) uses inverted Y clip coordinate
    frame_ubo.proj[1][1] *= -1;

    //  Copy frame UBO struct to uniform buffer
    frame_uniform.copy(frame_ubo);
}

//  ----------------------------------------------------------------------------
void task_update_glyph_uniforms(
    const std::vector<GlyphBatch>& batches,
    DynamicUniformBuffer<GlyphUbo>& glyph_uniform
) {
    size_t object_count = 0;
    for (const GlyphBatch& batch : batches) {
        object_count += batch.positions.size();
    }

    assert(object_count > 0);

    std::vector<GlyphUbo> data(object_count);

    size_t offset = 0;
    for (const GlyphBatch& batch : batches) {
        const size_t batch_object_count = batch.positions.size();

        assert(batch.sizes.size() == batch_object_count);
        assert(batch.bg_colors.size() == batch_object_count);
        assert(batch.fg_colors.size() == batch_object_count);

        for (size_t n = 0; n < batch_object_count; ++n) {
            const size_t m = n + offset;

            data[m].model =
                glm::translate(glm::mat4(1.0f), batch.positions[n]) *
                glm::scale(glm::mat4(1.0f), batch.sizes[n] * glm::vec3(0.5f, 0.5f, 1.0f));

            data[m].bg_color = batch.bg_colors[n];
            data[m].fg_color = batch.fg_colors[n];
            data[m].texture_index = batch.texture_id;
        }

        offset += batch_object_count;
    }

    //  Copy object UBO structs to dynamic uniform buffer
    glyph_uniform.copy(data);
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
