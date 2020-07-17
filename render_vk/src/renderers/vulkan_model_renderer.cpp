#include "render_vk/model_manager.hpp"
#include "render_vk/renderers/vulkan_model_renderer.hpp"
#include "render_vk/vulkan_model.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace render_vk
{
//  ----------------------------------------------------------------------------
VulkanModelRenderer::VulkanModelRenderer(ModelManager& model_mgr)
: m_model_mgr(model_mgr) {
}

//  ----------------------------------------------------------------------------
void VulkanModelRenderer::draw_models(
    const glm::mat4& view,
    const glm::mat4& proj,
    std::vector<uint32_t>& model_ids,
    std::vector<glm::vec3>& positions,
    std::vector<uint32_t>& texture_ids
) {
    // m_job_mgr.draw_models(model_ids, positions);

    m_view = view;

    //  GLM (OpenGL) uses inverted Y clip coordinate
    m_proj = proj;
    m_proj[1][1] *= -1;

    for (size_t n = 0; n < model_ids.size(); ++n) {
        VulkanModel* model = m_model_mgr.get_model(model_ids[n]);

        if (model == nullptr) {
            continue;
        }

        DrawModelCommand cmd{};
        cmd.texture_id = texture_ids[n];
        cmd.index_count = model->get_index_count();
        cmd.model = glm::translate(glm::mat4(1.0f), positions[n]);

        cmd.vertex_buffer = model->get_vertex_buffer();
        cmd.index_buffer = model->get_index_buffer();

        m_draw_commands.push_back(cmd);
    }
}
}
