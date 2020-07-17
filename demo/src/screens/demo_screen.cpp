#include "common/random.hpp"
#include "demo/screens/demo_screen.hpp"
#include "demo/ui/demo_state.hpp"
#include "demo/systems/demo_system.hpp"
#include "engine/engine.hpp"
#include "engine/game.hpp"
#include "engine/time.hpp"
#include "engine/ui/ui_state_manager.hpp"
#include "engine/system_manager.hpp"
#include "platform/window.hpp"
#include "render/renderer.hpp"
#include "render/renderers/model_renderer.hpp"
#include "systems/camera_system.hpp"
#include "systems/model_system.hpp"
#include "systems/move_system.hpp"
#include "systems/position_system.hpp"
#include "systems/system_util.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

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
}

//  ----------------------------------------------------------------------------
void DemoScreen::on_render(Game& game) {
    // const float elapsed_seconds = get_total_elapsed_seconds();

    SystemManager& sys_mgr = game.get_system_manager();

    //  Get active camera
    CameraSystem& camera_sys = get_camera_system(sys_mgr);
    if (!camera_sys.has_active_camera()) {
        return;
    }

    const Entity camera = camera_sys.get_active_camera();
    const auto camera_cmpnt = camera_sys.get_component(camera);
    glm::mat4 view = camera_sys.get_view_matrix(camera_cmpnt);

    Engine& engine = game.get_engine();
    Renderer& renderer = engine.get_renderer();
    const float aspect_ratio = renderer.get_aspect_ratio();

    glm::mat4 proj = glm::perspective(
        glm::radians(90.0f),
        aspect_ratio,
        0.1f,
        50.0f
    );

    Window& window = engine.get_window();
    renderer.begin_frame();

    glm::mat4 world = glm::mat4(1);

    //  Build draw order
    DemoSystem& demo_sys = sys_mgr.get_system<DemoSystem>(SYSTEM_ID_DEMO);
    std::vector<ModelBatch> model_batches;
    demo_sys.batch_models(game, model_batches);

    //  Draw entities
    ModelRenderer& model_renderer = renderer.get_model_renderer();
    model_renderer.draw_models(
        view,
        proj,
        model_batches
    );

    renderer.draw_frame(window.get_glfw_window());
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
