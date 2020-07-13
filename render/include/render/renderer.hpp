#pragma once

#include "common/asset.hpp"
#include "render/render_api.hpp"
#include <glm/fwd.hpp>
#include <memory>
#include <string>

struct GLFWwindow;

namespace render
{
class Model;

class Renderer
{
    RenderApi m_render_api;

public:
    Renderer(RenderApi render_api);
    virtual ~Renderer();
    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;
    virtual void begin_frame() = 0;
    virtual void draw_frame(GLFWwindow* glfw_window) = 0;
    virtual void draw_model(
        const common::AssetId id,
        const glm::mat4x4& model,
        const glm::mat4x4& view,
        const glm::mat4x4& proj
    ) = 0;

    RenderApi get_render_api() const {
        return m_render_api;
    }

    virtual float get_aspect_ratio() const = 0;
    virtual bool initialize(GLFWwindow* glfw_window) = 0;
    virtual void load_model(
        common::AssetId id,
        const std::string& path
    ) = 0;
    virtual void load_texture(
        common::AssetId id,
        const std::string& path
    ) = 0;
    virtual void resize(GLFWwindow* glfw_window) = 0;
    virtual void shutdown() = 0;
};
}
