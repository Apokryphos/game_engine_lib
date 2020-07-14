#pragma once

#include "render/renderer.hpp"

namespace render_gl
{
class GlRenderer : public render::Renderer
{
public:
    GlRenderer();
    virtual void draw_frame(GLFWwindow* glfw_window) override;
    virtual void begin_frame() override;
    virtual void draw_model(
        const common::AssetId model_id,
        const common::AssetId texture_id,
        const glm::mat4x4& model,
        const glm::mat4x4& view,
        const glm::mat4x4& proj
    ) override;
    virtual void draw_models(
        const glm::mat4& view,
        const glm::mat4& proj,
        std::vector<uint32_t>& model_ids,
        std::vector<glm::vec3>& positions
    ) override;
    virtual float get_aspect_ratio() const override;
    virtual bool initialize(GLFWwindow* glfw_window) override;
    virtual void load_model(
        common::AssetId id,
        const std::string& path
    ) override;
    virtual void load_texture(
        common::AssetId id,
        const std::string& path
    ) override;
    virtual void resize(GLFWwindow* glfw_window) override;
    virtual void shutdown() override;
};
}
