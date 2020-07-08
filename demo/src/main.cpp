#include "demo/screens/init_screen.hpp"
#include "engine/engine.hpp"
#include "engine/game.hpp"
#include "engine/screens/screen_manager.hpp"
#include "engine/system_manager.hpp"
#include "engine/systems/config_system.hpp"
#include "platform/window_options.hpp"

using namespace demo;
using namespace engine;
using namespace platform;

const std::string GAME_NAME = "game_engine_demo";
const std::string GAME_WINDOW_TITLE = "demo";

//  ----------------------------------------------------------------------------
static bool init_game(Game& game) {
    //  Default window options
    WindowOptions window_options{};
    window_options.title = GAME_WINDOW_TITLE;
    window_options.height = 800;
    window_options.width = 800;

    //  Create config system and load window options
    auto config_sys = std::make_unique<ConfigSystem>(GAME_NAME);
    config_sys->load_window_options(window_options);

    //  Initialize game
    if (!game.initialize(GAME_NAME, window_options)) {
        return false;
    }

    //  Add config system
    SystemManager& sys_mgr = game.get_system_manager();
    sys_mgr.add_system(std::move(config_sys));

    //  Load initialization screen
    Engine& engine = game.get_engine();
    ScreenManager& screen_mgr = engine.get_screen_manager();
    screen_mgr.add_screen(game, std::make_unique<InitScreen>());
    screen_mgr.activate(game, "init_screen");

    return true;
}

//  ----------------------------------------------------------------------------
int main(int argc, const char** argv) {
    Game game;
    if (!init_game(game)) {
        return -1;
    }

    game.run();
    game.shutdown();

    return 0;
}
