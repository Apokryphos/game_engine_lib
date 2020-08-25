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
    const GlyphBatch& glyph_batch,
    DynamicUniformBuffer<GlyphUbo>& glyph_uniform
) {
    size_t object_count = glyph_batch.get_instance_count();
    assert(object_count > 0);

    std::vector<GlyphUbo> data(object_count);

    const auto& batches = glyph_batch.get_batches();
    const std::vector<GlyphBatch::Glyph>& glyphs = glyph_batch.get_glyphs();

    size_t ubo_index = 0;
    const glm::mat4 identity(1.0f);
    for (const auto& batch : batches) {
        for (size_t n = batch.start; n < batch.end; ++n) {
            const GlyphBatch::Glyph& glyph = glyphs[n];

            data[ubo_index].texture_index = batch.texture_id;

            data[ubo_index].model = glm::scale(
                glm::translate(identity, glyph.position),
                glm::vec3(glyph.size, 1.0f)
            );

            data[ubo_index].bg_color = glyph.bg_color;
            data[ubo_index].fg_color = glyph.fg_color;

            ++ubo_index;
        }
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
