#include "common/log.hpp"
#include "ecs/ecs_root.hpp"
#include "engine/engine.hpp"
#include "engine/game.hpp"
#include "engine/imgui/imgui_base.hpp"
#include "engine/screens/screen_manager.hpp"
#include "engine/system_manager.hpp"
#include "engine/base_systems/base_system_util.hpp"
#include "engine/base_systems/config_system.hpp"
#include "engine/base_systems/debug_gui_system.hpp"
#include "engine/base_systems/editor_system.hpp"
#include "engine/base_systems/profile_system.hpp"
#include "engine/time.hpp"
#include "engine/ui/ui_state_manager.hpp"
#include "filesystem/paths.hpp"
#include "input/input_manager.hpp"
#include "platform/window.hpp"
#include "render/renderer.hpp"
#include "render_vk/debug_gui/vulkan_debug_panel.hpp"
#include "render_vk/vulkan_render_system.hpp"
#include <cassert>

using namespace common;
using namespace ecs;
using namespace filesystem;
using namespace input;
using namespace platform;
using namespace render;
using namespace render_vk;

namespace engine
{
//  ----------------------------------------------------------------------------
Game::Game(const std::string& game_base_name, const size_t max_entities)
: m_max_entities(max_entities),
  m_ecs_root(std::make_unique<EcsRoot>()),
  m_engine(std::make_unique<Engine>()),
  m_sys_mgr(nullptr) {
    //  Any log calls before here will not be written to the log file.
    initialize_log(get_game_log_path(game_base_name));

    m_sys_mgr = std::make_unique<SystemManager>(*m_ecs_root);

    initialize_base_systems(*this, game_base_name, max_entities);
}

//  ----------------------------------------------------------------------------
Game::~Game() {
}

//  ----------------------------------------------------------------------------
EcsRoot& Game::get_ecs_root() {
    assert(m_ecs_root != nullptr);
    return *m_ecs_root;
}

//  ----------------------------------------------------------------------------
Engine& Game::get_engine() {
    assert(m_engine != nullptr);
    return *m_engine;
}

//  ----------------------------------------------------------------------------
SystemManager& Game::get_system_manager() {
    assert(m_sys_mgr != nullptr);
    return *m_sys_mgr;
}

//  ----------------------------------------------------------------------------
bool Game::initialize(
    const std::string& title,
    WindowOptions window_options
) {
    log_debug("Initializing...");

    //  Load window options from configuration
    ConfigSystem& config_sys = get_config_system(*m_sys_mgr);
    config_sys.load_window_options(window_options);

    //  Initialize engine
    if (!m_engine->initialize(title, window_options, m_max_entities)) {
        return false;
    }

    //  Add Vulkan render system debug GUI panel
    Renderer& render_sys = m_engine->get_render_system();
    if (render_sys.get_render_api() == RenderApi::Vulkan) {
        get_debug_gui_system(*m_sys_mgr).add_gui(
            "vulkan",
            make_vulkan_debug_panel_function(
                dynamic_cast<VulkanRenderSystem&>(render_sys).get_instance()
            )
        );
    }

    return true;
}

//  ----------------------------------------------------------------------------
void Game::quit() {
    m_quit = true;
}

//  ----------------------------------------------------------------------------
void Game::render() {
    Renderer& render_sys = m_engine->get_render_system();
    render_sys.begin_frame();

    //  Render screen
    ScreenManager& screen_mgr = m_engine->get_screen_manager();
    screen_mgr.render(*this);

    render_sys.end_frame();
}

//  ----------------------------------------------------------------------------
void Game::run() {
    log_debug("Entering main loop...");

    while (!m_quit) {
        update_time();

        //  Update input
        InputManager& input_mgr = m_engine->get_input_manager();
        input_mgr.poll();

        //  Check if window should close
        Window& window = m_engine->get_window();
        if (window.should_close()) {
            quit();
        }

        update();
        render();
    }
}

//  ----------------------------------------------------------------------------
void Game::shutdown() {
    log_debug("Shutting down...");

    m_engine->shutdown();
}

//  ----------------------------------------------------------------------------
void Game::update() {
    //  Update UI
    InputManager& input_mgr = m_engine->get_input_manager();
    UiStateManager& ui_state_mgr = m_engine->get_ui_state_manager();

    InputEvent event;
    while (input_mgr.consume_event(event)) {
        ui_state_mgr.process_event(*this, event);
    }

    //  Update screen
    ScreenManager& screen_mgr = m_engine->get_screen_manager();
    screen_mgr.update(*this);

    //  Start a new ImGui frame. This shoud be called as early as possible,
    //  but ImGui GUI calls should be restricted to DebugGuiSystem.
    const RenderApi render_api = m_engine->get_render_system().get_render_api();
    imgui_new_frame(render_api);

    //  Update debug GUI
    DebugGuiSystem& debug_gui_system = get_debug_gui_system(*m_sys_mgr);
    debug_gui_system.update(*this);

    //  Update editor GUI
    EditorSystem& editor_system = get_editor_system(*m_sys_mgr);
    editor_system.update(*this);
}
}
