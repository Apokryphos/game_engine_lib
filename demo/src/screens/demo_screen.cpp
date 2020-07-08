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
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace common;
using namespace engine;
using namespace platform;
using namespace render;

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

    Random& random = game.get_random();
    std::uniform_int_distribution<int> dist(-10, 10);

    for (int n = 0; n < 100; ++n) {
        glm::vec3 pos = {
            dist(random.get_rng()),
            dist(random.get_rng()),
            0.0f
        };

        m_positions.push_back(pos);
    }
}

//  ----------------------------------------------------------------------------
void DemoScreen::on_render(Game& game) {
    // const float elapsed_seconds = get_total_elapsed_seconds();

    SystemManager& sys_mgr = game.get_system_manager();
    DemoSystem& demo_sys = sys_mgr.get_system<DemoSystem>(SYSTEM_ID_DEMO);

    glm::mat4 rotate = glm::rotate(
        glm::mat4(1.0f),
        demo_sys.get_rotate(),
        glm::vec3(0.0f, 0.0f, 1.0f)
    );

    glm::mat4 world = rotate;

    const glm::vec3 camera_pos = demo_sys.get_position();
    glm::mat4 view = glm::lookAt(
        camera_pos,
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 1.0f)
    );

    Engine& engine = game.get_engine();
    Renderer& renderer = engine.get_renderer();
    const float aspect_ratio = renderer.get_aspect_ratio();

    const float fov = demo_sys.get_fov();

    glm::mat4 proj = glm::perspective(
        glm::radians(fov),
        aspect_ratio,
        0.1f,
        20.0f
    );

    Window& window = engine.get_window();
    renderer.begin_frame();

    for (const glm::vec3& pos : m_positions) {
        glm::mat4 model = glm::translate(glm::mat4(1.0f), pos);
        renderer.draw_model(1, model * world, view, proj);
    }

    renderer.draw_frame(window.get_glfw_window());
}

//  ----------------------------------------------------------------------------
void DemoScreen::on_update(Game& game) {
    SystemManager& sys_mgr = game.get_system_manager();
    DemoSystem& demo_sys = sys_mgr.get_system<DemoSystem>(SYSTEM_ID_DEMO);
    demo_sys.update(game);
}
}
