#pragma once

#include "render/renderers/model_renderer.hpp"
#include "render_vk/command_buffer.hpp"

namespace render_vk
{
class ModelManager;

class VulkanModelRenderer : public render::ModelRenderer
{
    glm::mat4 m_view;
    glm::mat4 m_proj;

    //  Draw commands
    //  TODO: Replace with secondary command buffer
    std::vector<DrawModelCommand> m_draw_commands;

    ModelManager& m_model_mgr;

public:
    VulkanModelRenderer(ModelManager& model_mgr);

    void begin_frame() {
        m_draw_commands.clear();
    }

    virtual void draw_models(
        const glm::mat4& view,
        const glm::mat4& proj,
        std::vector<uint32_t>& model_ids,
        std::vector<glm::vec3>& positions,
        std::vector<uint32_t>& texture_ids
    ) override;

    const std::vector<DrawModelCommand>& get_draw_commands() const {
        return m_draw_commands;
    }

    glm::mat4 get_projection() const {
        return m_proj;
    }

    glm::mat4 get_view() const {
        return m_view;
    }
};
}
