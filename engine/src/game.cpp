#include "common/log.hpp"
#include "ecs/ecs_root.hpp"
#include "engine/engine.hpp"
#include "engine/game.hpp"
#include "engine/screens/screen_manager.hpp"
#include "engine/system_manager.hpp"
#include "engine/time.hpp"
#include "engine/ui/ui_state_manager.hpp"
#include "input/input_manager.hpp"
#include "platform/window.hpp"
#include "render/renderer.hpp"
#include <cassert>

using namespace common;
using namespace ecs;
using namespace input;
using namespace platform;
using namespace render;

namespace engine
{
//  ----------------------------------------------------------------------------
Game::Game()
: m_quit(false),
  m_ecs_root(std::make_unique<EcsRoot>()),
  m_engine(std::make_unique<Engine>()),
  m_sys_mgr(nullptr) {
    m_sys_mgr = std::make_unique<SystemManager>(*m_ecs_root);
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
    const WindowOptions& window_options
) {
    log_debug("Initializing...");

    if (!m_engine->initialize(title, window_options)) {
        return false;
    }

    return true;
}

//  ----------------------------------------------------------------------------
void Game::quit() {
    m_quit = true;
}

//  ----------------------------------------------------------------------------
void Game::render() {
    //  Render screen
    ScreenManager& screen_mgr = m_engine->get_screen_manager();
    screen_mgr.render(*this);
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
}
}
