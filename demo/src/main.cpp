#include "demo/screens/init_screen.hpp"
#include "engine/engine.hpp"
#include "engine/game.hpp"
#include "engine/screens/screen_manager.hpp"
#include "platform/window_options.hpp"

using namespace demo;
using namespace engine;
using namespace platform;

//  This is the name used for the base config/save directory
const std::string GAME_BASE_NAME = "game_engine_demo";
const std::string GAME_WINDOW_TITLE = "demo";

//  ----------------------------------------------------------------------------
static bool init_game(Game& game) {
    //  Default window options
    WindowOptions window_options{};
    window_options.title = GAME_WINDOW_TITLE;
    window_options.height = 800;
    window_options.width = 800;

    //  Initialize game
    if (!game.initialize(GAME_BASE_NAME, window_options)) {
        return false;
    }

    //  Load initialization screen
    Engine& engine = game.get_engine();
    ScreenManager& screen_mgr = engine.get_screen_manager();
    screen_mgr.add_screen(game, std::make_unique<InitScreen>());
    screen_mgr.activate(game, "init_screen");

    return true;
}

//  ----------------------------------------------------------------------------
int main(int argc, const char** argv) {
    Game game(GAME_BASE_NAME);
    if (!init_game(game)) {
        return -1;
    }

    game.run();
    game.shutdown();

    return 0;
}
