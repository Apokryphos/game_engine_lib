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
        const common::AssetId id,
        const glm::mat4x4& model,
        const glm::mat4x4& view,
        const glm::mat4x4& proj
    ) override;
    virtual float get_aspect_ratio() const override;
    virtual bool initialize(GLFWwindow* glfw_window) override;
    virtual void load_model(
        common::AssetId id,
        const std::string& path
    ) override;
    virtual void resize(GLFWwindow* glfw_window) override;
    virtual void shutdown() override;
};
}
