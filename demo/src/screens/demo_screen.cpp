#include "assets/asset_manager.hpp"
#include "common/random.hpp"
#include "demo/screens/demo_screen.hpp"
#include "demo/ui/demo_state.hpp"
#include "demo/systems/demo_system.hpp"
#include "engine/base_systems/name_system.hpp"
#include "engine/engine.hpp"
#include "engine/game.hpp"
#include "engine/time.hpp"
#include "engine/ui/ui_state_manager.hpp"
#include "engine/system_manager.hpp"
#include "platform/window.hpp"
#include "render/renderer.hpp"
#include "systems/camera_system.hpp"
#include "systems/model_system.hpp"
#include "systems/move_system.hpp"
#include "systems/position_system.hpp"
#include "systems/system_util.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace assets;
using namespace ecs;
using namespace common;
using namespace engine;
using namespace platform;
using namespace render;
using namespace systems;

namespace demo
{
//  ----------------------------------------------------------------------------
void DemoScreen::on_activate(Game& game) {
    //  Activate UI state
    Engine& engine = game.get_engine();
    UiStateManager& ui_state_mgr = engine.get_ui_state_manager();
    ui_state_mgr.activate(game, "demo_state");
}

//  ----------------------------------------------------------------------------
void DemoScreen::on_load(Game& game) {
    //  Load UI state
    Engine& engine = game.get_engine();
    UiStateManager& ui_state_mgr = engine.get_ui_state_manager();
    ui_state_mgr.add_state(game, std::make_unique<DemoState>());

    AssetManager& asset_mgr = game.get_engine().get_asset_manager();

    //  Create glyph mesh
    GlyphMeshCreateArgs args {};
    const glm::vec3 size = glm::vec3(20.0f, 20.0f, 0.0f);

    Random& random = game.get_random();
    std::uniform_int_distribution<int> glyph_dist(0, 255);

    for (float y = 0; y < 80; ++y) {
        for (float x = 0; x < 192; ++x) {
            GlyphMeshCreateArgs::Glyph glyph;
            glyph.position = glm::vec3(x, y, 0.0f) * size;
            glyph.size = size;
            glyph.bg_color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
            glyph.fg_color = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
            glyph.texture_id = 7 +  glyph_dist(random.get_rng());

            args.glyphs.push_back(glyph);
        }
    }
    m_glyph_mesh = asset_mgr.create_glyph_mesh(args);
}

//  ----------------------------------------------------------------------------
void DemoScreen::on_render(Game& game) {
    // const float elapsed_seconds = get_total_elapsed_seconds();

    SystemManager& sys_mgr = game.get_system_manager();
    NameSystem& name_sys = get_name_system(sys_mgr);

    //  Get active camera
    CameraSystem& camera_sys = get_camera_system(sys_mgr);
    if (!camera_sys.has_active_camera()) {
        return;
    }

    //  Perspective camera
    const Entity camera = name_sys.get_entity("camera");
    const auto camera_cmpnt = camera_sys.get_component(camera);
    glm::mat4 view = camera_sys.get_view_matrix(camera_cmpnt);

    Engine& engine = game.get_engine();
    Renderer& render_sys = engine.get_render_system();
    const float aspect_ratio = render_sys.get_aspect_ratio();
    const glm::vec2 size = render_sys.get_size();

    const glm::mat4 proj = glm::perspective(
        glm::radians(90.0f),
        aspect_ratio,
        0.1f,
        256.0f
    );

    //  Orthographic camera
    const Entity ortho_camera = name_sys.get_entity("ortho_camera");
    const auto ortho_camera_cmpnt = camera_sys.get_component(ortho_camera);
    const glm::mat4 ortho_view = camera_sys.get_view_matrix(ortho_camera_cmpnt);
    const glm::mat4 ortho_proj = glm::ortho(
        0.0f,
        size.x,
        0.0f,
        size.y,
        -1.0f,
        1.0f
    );

    render_sys.update_frame_uniforms(view, proj, ortho_view, ortho_proj);

    DemoSystem& demo_sys = sys_mgr.get_system<DemoSystem>(SYSTEM_ID_DEMO);

    //  Batch models
    std::vector<ModelBatch> model_batches;
    demo_sys.batch_models(game, view, proj, model_batches);
    render_sys.draw_models(model_batches);

    //  Batch billboards
    std::vector<SpriteBatch> billboard_batches;
    demo_sys.batch_billboards(game, view, proj, billboard_batches);
    render_sys.draw_billboards(billboard_batches);

    //  Batch Spine sprites
    std::vector<SpineSpriteBatch> spine_sprite_batches;
    demo_sys.batch_spines(game, view, proj, spine_sprite_batches);
    render_sys.draw_spines(spine_sprite_batches);

    //  Batch sprites
    std::vector<SpriteBatch> sprite_batches;
    demo_sys.batch_sprites(game, ortho_view, ortho_proj, sprite_batches);
    render_sys.draw_sprites(sprite_batches);

    //  Batch glyphs
    GlyphBatch glyph_batch;
    demo_sys.batch_glyphs(game, ortho_view, ortho_proj, glyph_batch);
    render_sys.draw_glyphs(glyph_batch);

    render_sys.draw_glyph_mesh(m_glyph_mesh);
}

//  ----------------------------------------------------------------------------
void DemoScreen::on_update(Game& game) {
    SystemManager& sys_mgr = game.get_system_manager();

    MoveSystem& move_sys = get_move_system(sys_mgr);
    move_sys.update(game);

    CameraSystem& camera_sys = get_camera_system(sys_mgr);
    camera_sys.update(game);

    // DemoSystem& demo_sys = sys_mgr.get_system<DemoSystem>(SYSTEM_ID_DEMO);
    // demo_sys.update(game);
}
}
