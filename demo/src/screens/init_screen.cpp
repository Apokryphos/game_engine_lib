#include "common/log.hpp"
#include "demo/input/input_action_ids.hpp"
#include "demo/screens/demo_screen.hpp"
#include "demo/screens/init_screen.hpp"
#include "demo/systems/demo_system.hpp"
#include "ecs/ecs_root.hpp"
#include "engine/asset_manager.hpp"
#include "engine/engine.hpp"
#include "engine/game.hpp"
#include "engine/screens/screen_manager.hpp"
#include "engine/system_manager.hpp"
#include "input/gamepad.hpp"
#include "input/input_manager.hpp"
#include "render/renderer.hpp"
#include "systems/config_system.hpp"
#include "systems/name_system.hpp"
#include "systems/profile_system.hpp"
#include "systems/system_util.hpp"

using namespace common;
using namespace demo;
using namespace ecs;
using namespace engine;
using namespace input;
using namespace render;
using namespace systems;

namespace demo
{
//  ----------------------------------------------------------------------------
static void init_input_actions(InputManager& input_mgr) {
    log_debug("Initializing input actions...");

    InputActionSet& action_set = input_mgr.get_action_set();
    action_set.add_action(INPUT_ACTION_ID_QUIT, InputType::Digital, InputEventType::Pressed);
    action_set.add_action(INPUT_ACTION_ID_ACCEPT, InputType::Digital, InputEventType::Pressed);
    action_set.add_action(INPUT_ACTION_ID_CANCEL, InputType::Digital, InputEventType::Delta);
    action_set.add_action(INPUT_ACTION_ID_MOVE_UP, InputType::Analog, InputEventType::Down);
    action_set.add_action(INPUT_ACTION_ID_MOVE_DOWN, InputType::Analog, InputEventType::Down);
    action_set.add_action(INPUT_ACTION_ID_MOVE_RIGHT, InputType::Analog, InputEventType::Down);
    action_set.add_action(INPUT_ACTION_ID_MOVE_LEFT, InputType::Analog, InputEventType::Down);
    action_set.add_action(INPUT_ACTION_ID_AIM_HORZ, InputType::Analog, InputEventType::Delta);
    action_set.add_action(INPUT_ACTION_ID_AIM_VERT, InputType::Analog, InputEventType::Delta);
    action_set.add_action(INPUT_ACTION_ID_FIRE, InputType::Digital, InputEventType::Delta);
    action_set.add_action(INPUT_ACTION_ID_ZOOM_IN, InputType::Analog, InputEventType::Down);
    action_set.add_action(INPUT_ACTION_ID_ZOOM_OUT, InputType::Analog, InputEventType::Down);
    action_set.add_action(INPUT_ACTION_ID_ROTATE_CW, InputType::Analog, InputEventType::Down);
    action_set.add_action(INPUT_ACTION_ID_ROTATE_CCW, InputType::Analog, InputEventType::Down);
}

//  ----------------------------------------------------------------------------
static void init_key_binds(InputManager& input_mgr) {
    log_debug("Initializing key bindings...");

    Keyboard& keyboard = input_mgr.get_keyboard();
    InputBindMap& bind_map = keyboard.get_map();
    bind_map.bind_key(INPUT_ACTION_ID_QUIT, GLFW_KEY_ESCAPE);
    bind_map.bind_key(INPUT_ACTION_ID_ACCEPT, GLFW_KEY_ENTER);
    bind_map.bind_key(INPUT_ACTION_ID_CANCEL, GLFW_KEY_BACKSPACE);
    bind_map.bind_key(INPUT_ACTION_ID_MOVE_UP, GLFW_KEY_UP);
    bind_map.bind_key(INPUT_ACTION_ID_MOVE_DOWN, GLFW_KEY_DOWN);
    bind_map.bind_key(INPUT_ACTION_ID_MOVE_RIGHT, GLFW_KEY_RIGHT);
    bind_map.bind_key(INPUT_ACTION_ID_MOVE_LEFT, GLFW_KEY_LEFT);
    bind_map.bind_key(INPUT_ACTION_ID_ROTATE_CW, GLFW_KEY_END);
    bind_map.bind_key(INPUT_ACTION_ID_ROTATE_CCW, GLFW_KEY_HOME);
    bind_map.bind_key(INPUT_ACTION_ID_ZOOM_IN, GLFW_KEY_EQUAL);
    bind_map.bind_key(INPUT_ACTION_ID_ZOOM_OUT, GLFW_KEY_MINUS);
}

//  ----------------------------------------------------------------------------
static void init_gamepad_binds(InputManager& input_mgr) {
    log_debug("Initializing gamepad bindings...");

    Gamepad& gamepad = input_mgr.get_gamepad(0);
    InputBindMap& bind_map = gamepad.get_map();
    bind_map.bind_button(INPUT_ACTION_ID_QUIT, GLFW_GAMEPAD_BUTTON_START);
    bind_map.bind_button(INPUT_ACTION_ID_ACCEPT, GLFW_GAMEPAD_BUTTON_CROSS);
    bind_map.bind_button(INPUT_ACTION_ID_CANCEL, GLFW_GAMEPAD_BUTTON_CIRCLE);
    bind_map.bind_button(INPUT_ACTION_ID_MOVE_RIGHT, GLFW_GAMEPAD_BUTTON_DPAD_RIGHT);
    bind_map.bind_button(INPUT_ACTION_ID_MOVE_LEFT, GLFW_GAMEPAD_BUTTON_DPAD_LEFT);

    bind_map.bind_axis(INPUT_ACTION_ID_MOVE_RIGHT, GLFW_GAMEPAD_AXIS_LEFT_X, AxisSign::Positive);
    bind_map.bind_axis(INPUT_ACTION_ID_MOVE_LEFT, GLFW_GAMEPAD_AXIS_LEFT_X, AxisSign::Negative);

    bind_map.bind_axis(INPUT_ACTION_ID_AIM_HORZ, GLFW_GAMEPAD_AXIS_RIGHT_X);
    bind_map.bind_axis(INPUT_ACTION_ID_AIM_VERT, GLFW_GAMEPAD_AXIS_RIGHT_Y);

    bind_map.bind_axis(INPUT_ACTION_ID_FIRE, GLFW_GAMEPAD_AXIS_LEFT_TRIGGER);
}

//  ----------------------------------------------------------------------------
static void init_systems(Game& game) {
    SystemManager& sys_mgr = game.get_system_manager();
    sys_mgr.add_system(std::make_unique<ProfileSystem>("game_engine_demo"));
    sys_mgr.add_system(std::make_unique<DemoSystem>());

    get_profile_system(sys_mgr).load_profiles();
}

//  ----------------------------------------------------------------------------
static void init_ecs(Game& game) {
    EcsRoot& ecs = game.get_ecs_root();
    static std::unique_ptr<NameSystem> name_sys = std::make_unique<NameSystem>(ecs, 1000);
    ecs.add_system(name_sys.get());

    SystemManager& sys_mgr = game.get_system_manager();
    sys_mgr.add_system(std::move(name_sys));

    Entity entity = ecs.create_entity();
    NameSystem& ns = sys_mgr.get_system<NameSystem>(SYSTEM_ID_NAME);
    ns.add_component(entity);
    const auto name_cmpnt = ns.get_component(entity);
    ns.set_name(name_cmpnt, "Actor");
}

//  ----------------------------------------------------------------------------
static void load_demo_screen(Game& game) {
    Engine& engine = game.get_engine();

    ScreenManager& screen_mgr = engine.get_screen_manager();
    screen_mgr.add_screen(game, std::make_unique<DemoScreen>());
    screen_mgr.activate(game, "demo_screen");
}

//  ----------------------------------------------------------------------------
void InitScreen::on_load(Game& game) {
    Engine& engine = game.get_engine();

    //  Initialize input
    InputManager& input_mgr = engine.get_input_manager();
    init_input_actions(input_mgr);
    init_key_binds(input_mgr);
    init_gamepad_binds(input_mgr);

    //  Initialize systems
    init_systems(game);

    //  Initialize ECS
    init_ecs(game);

    //  Load assets
    AssetManager& asset_mgr = engine.get_asset_manager();
    Renderer& renderer = engine.get_renderer();
    asset_mgr.load_model(renderer, "assets/models/model.obj");

    //  Next screen
    load_demo_screen(game);
}
}
