#pragma once

#include "render/glyph_batch.hpp"
#include "render/model_batch.hpp"
#include "render/render_api.hpp"
#include "render/sprite_batch.hpp"
#include "render/spine_sprite_batch.hpp"
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <memory>
#include <string>
#include <vector>

struct GLFWwindow;

namespace render
{
class Model;
class ModelRenderer;

class Renderer
{
    RenderApi m_render_api;

public:
    Renderer(RenderApi render_api);
    virtual ~Renderer();
    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;
    virtual void begin_frame() = 0;
    virtual void draw_billboards(
        std::vector<SpriteBatch>& batches
    ) = 0;
    virtual void draw_glyph_mesh(assets::AssetId glyph_mesh_id) = 0;
    virtual void draw_glyphs(GlyphBatch& glyph_batch) = 0;
    virtual void draw_models(
        std::vector<ModelBatch>& batches
    ) = 0;
    virtual void draw_spines(
        std::vector<SpineSpriteBatch>& batches
    ) = 0;
    virtual void draw_sprites(
        std::vector<SpriteBatch>& batches
    ) = 0;
    virtual void end_frame() = 0;

    RenderApi get_render_api() const {
        return m_render_api;
    }

    virtual float get_aspect_ratio() const = 0;
    virtual glm::vec2 get_size() const = 0;
    virtual bool initialize(GLFWwindow* glfw_window) = 0;
    virtual void resize() = 0;
    virtual void shutdown() = 0;
    virtual void update_frame_uniforms(
        const glm::mat4& view,
        const glm::mat4& proj,
        const glm::mat4& ortho_view,
        const glm::mat4& ortho_proj
    ) = 0;
};
}
