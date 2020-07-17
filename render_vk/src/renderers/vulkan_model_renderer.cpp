#include "render_vk/model_manager.hpp"
#include "render_vk/renderers/vulkan_model_renderer.hpp"
#include "render_vk/vulkan_model.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace render_vk
{
//  Number of objects to create UBO data structs for
const size_t OBJECT_INSTANCES = 100;

//  ----------------------------------------------------------------------------
VulkanModelRenderer::VulkanModelRenderer(ModelManager& model_mgr)
: m_model_mgr(model_mgr),
  m_object_uniform(OBJECT_INSTANCES) {
}

//  ----------------------------------------------------------------------------
void VulkanModelRenderer::create_objects(
    VkPhysicalDevice physical_device,
    VkDevice device
) {
    m_device = device;
    m_frame_uniform.create(physical_device, m_device);
    m_object_uniform.create(physical_device, m_device);
}

//  ----------------------------------------------------------------------------
void VulkanModelRenderer::destroy_objects() {
    m_frame_uniform.destroy();
    m_object_uniform.destroy();
    m_device = VK_NULL_HANDLE;
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

    update_uniform_buffers(view, proj, positions, texture_ids);

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

//  ----------------------------------------------------------------------------
void VulkanModelRenderer::update_uniform_buffers(
    glm::mat4 view,
    glm::mat4 proj,
    const std::vector<glm::vec3>& positions,
    const std::vector<uint32_t>& texture_ids
) {
    assert(positions.size() == texture_ids.size());

    //  Update frame UBO
    FrameUbo frame_ubo{};
    frame_ubo.view = view;
    frame_ubo.proj = proj;

    //  GLM (OpenGL) uses inverted Y clip coordinate
    frame_ubo.proj[1][1] *= -1;

    //  Copy frame UBO struct to uniform buffer
    m_frame_uniform.copy(frame_ubo);

    //  Update all UBO structs once per frame
    const size_t object_count = positions.size();
    assert(object_count > 0);

    std::vector<ObjectUbo> data(object_count);
    for (size_t n = 0; n < object_count; ++n)  {
        data[n].texture_index = texture_ids[n];
        data[n].model = glm::translate(glm::mat4(1.0f), positions[n]);
    }

    //  Copy object UBO structs to dynamic uniform buffer
    m_object_uniform.copy(data);
}
}
