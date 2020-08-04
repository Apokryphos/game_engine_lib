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
    virtual float get_aspect_ratio() const override;
    virtual render::ModelRenderer& get_model_renderer() override;
    virtual bool initialize(GLFWwindow* glfw_window) override;
    virtual void load_model(
        assets::AssetId id,
        const std::string& path
    ) override;
    virtual void load_texture(
        assets::AssetId id,
        const std::string& path,
        const render::TextureCreateArgs& args
    ) override;
    virtual void resize(GLFWwindow* glfw_window) override;
    virtual void shutdown() override;
};
}
