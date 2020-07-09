#include "common/log.hpp"
#include "demo/input/input_action_ids.hpp"
#include "demo/screens/demo_screen.hpp"
#include "demo/screens/init_screen.hpp"
#include "demo/systems/demo_system.hpp"
#include "ecs/ecs_root.hpp"
#include "engine/asset_manager.hpp"
#include "engine/base_systems/debug_gui_util.hpp"
#include "engine/base_systems/config_system.hpp"
#include "engine/base_systems/profile_system.hpp"
#include "engine/engine.hpp"
#include "engine/game.hpp"
#include "engine/screens/screen_manager.hpp"
#include "engine/system_manager.hpp"
#include "input/gamepad.hpp"
#include "input/input_manager.hpp"
#include "render/renderer.hpp"
#include "systems/debug_gui/name_system_debug_gui.hpp"
#include "systems/name_system.hpp"
#include "systems/position_system.hpp"
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
    action_set.add_action(INPUT_ACTION_ID_TOGGLE_DEBUG_GUI, "Toggle Debug GUI", InputType::Digital, InputEventType::Pressed);
    action_set.add_action(INPUT_ACTION_ID_QUIT, "Quit", InputType::Digital, InputEventType::Pressed);
    action_set.add_action(INPUT_ACTION_ID_ACCEPT, "Accept", InputType::Digital, InputEventType::Pressed);
    action_set.add_action(INPUT_ACTION_ID_CANCEL, "Cancel", InputType::Digital, InputEventType::Pressed);
    action_set.add_action(INPUT_ACTION_ID_MOVE_UP, "Move Up", InputType::Analog, InputEventType::Down);
    action_set.add_action(INPUT_ACTION_ID_MOVE_DOWN, "Move Down", InputType::Analog, InputEventType::Down);
    action_set.add_action(INPUT_ACTION_ID_MOVE_RIGHT, "Move Right", InputType::Analog, InputEventType::Down);
    action_set.add_action(INPUT_ACTION_ID_MOVE_LEFT, "Move Left", InputType::Analog, InputEventType::Down);
    action_set.add_action(INPUT_ACTION_ID_AIM_HORZ, "Aim X", InputType::Analog, InputEventType::Delta);
    action_set.add_action(INPUT_ACTION_ID_AIM_VERT, "Aim Y", InputType::Analog, InputEventType::Delta);
    action_set.add_action(INPUT_ACTION_ID_FIRE, "Fire", InputType::Digital, InputEventType::Delta);
    action_set.add_action(INPUT_ACTION_ID_ZOOM_IN, "Zoom In", InputType::Analog, InputEventType::Down);
    action_set.add_action(INPUT_ACTION_ID_ZOOM_OUT, "Zoom Out", InputType::Analog, InputEventType::Down);
    action_set.add_action(INPUT_ACTION_ID_ROTATE_CW, "Rotate Left", InputType::Analog, InputEventType::Down);
    action_set.add_action(INPUT_ACTION_ID_ROTATE_CCW, "Rotate Right", InputType::Analog, InputEventType::Down);
}

//  ----------------------------------------------------------------------------
static void init_default_key_binds(InputManager& input_mgr) {
    log_debug("Initializing key bindings...");

    Keyboard& keyboard = input_mgr.get_keyboard();
    InputBindMap& bind_map = keyboard.get_map();
    bind_map.bind_key(INPUT_ACTION_ID_TOGGLE_DEBUG_GUI, GLFW_KEY_GRAVE_ACCENT);
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
static void init_default_mouse_binds(InputManager& input_mgr) {
    log_debug("Initializing mouse bindings...");

    Mouse& mouse = input_mgr.get_mouse();
    InputBindMap& bind_map = mouse.get_map();
    bind_map.bind_mouse_button(INPUT_ACTION_ID_ACCEPT, GLFW_MOUSE_BUTTON_LEFT);
    bind_map.bind_mouse_button(INPUT_ACTION_ID_CANCEL, GLFW_MOUSE_BUTTON_RIGHT);
}

//  ----------------------------------------------------------------------------
static void init_default_gamepad_binds(InputManager& input_mgr) {
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
static void init_input(Game& game) {
    Engine& engine = game.get_engine();

    InputManager& input_mgr = engine.get_input_manager();

    //  Initialize all input actions used by this application
    init_input_actions(input_mgr);

    //  Initialize default input binds
    init_default_key_binds(input_mgr);
    init_default_mouse_binds(input_mgr);
    init_default_gamepad_binds(input_mgr);

    //  Load input bindings configuration after game initializes
    ConfigSystem& config_sys = get_config_system(game.get_system_manager());
    config_sys.load_input_bindings(
        input_mgr.get_keyboard().get_map(),
        input_mgr.get_mouse().get_map(),
        input_mgr.get_gamepad(0).get_map()
    );

    //  Set input maps merged from defaults and configs
    input_mgr.get_keyboard().set_map(config_sys.get_keyboard_binds());
    input_mgr.get_mouse().set_map(config_sys.get_mouse_binds());
    input_mgr.set_gamepad_maps(config_sys.get_gamepad_binds());
}

//  ----------------------------------------------------------------------------
static void init_systems(Game& game) {
    SystemManager& sys_mgr = game.get_system_manager();
    sys_mgr.add_system(std::make_unique<DemoSystem>());

    get_profile_system(sys_mgr).load_profiles();
}

//  ----------------------------------------------------------------------------
static void init_ecs_systems(Game& game) {
    EcsRoot& ecs = game.get_ecs_root();
    SystemManager& sys_mgr = game.get_system_manager();

    sys_mgr.add_system(std::make_unique<NameSystem>(ecs, 1000));
    add_debug_gui<NameSystem>(game, get_name_system(sys_mgr), name_system_debug_gui);

    sys_mgr.add_system(std::make_unique<PositionSystem>(ecs, 1000));
}

//  ----------------------------------------------------------------------------
static void init_entities(Game& game) {
    EcsRoot& ecs = game.get_ecs_root();
    SystemManager& sys_mgr = game.get_system_manager();

    //  Position distribution
    Random& random = game.get_random();
    std::uniform_int_distribution<int> position_dist(-10, 10);

    const int ENTITY_COUNT = 100;
    for (int n = 0; n < ENTITY_COUNT; ++n) {
        Entity entity = ecs.create_entity();

        const std::string name = "entity_" + std::to_string(n);

        NameSystem& name_sys = get_name_system(sys_mgr);
        name_sys.add_component(entity);
        const auto name_cmpnt = name_sys.get_component(entity);
        name_sys.set_name(name_cmpnt, name);

        const glm::vec3 position = {
            position_dist(random.get_rng()),
            position_dist(random.get_rng()),
            0.0f
        };

        PositionSystem& pos_sys = get_position_system(sys_mgr);
        pos_sys.add_component(entity);
        const auto pos_cmpnt = pos_sys.get_component(entity);
        pos_sys.set_position(pos_cmpnt, position);
    }
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
    init_input(game);

    //  Initialize systems
    init_systems(game);

    //  Initialize ECS systems
    init_ecs_systems(game);

    //  Initialize entities
    init_entities(game);

    //  Load assets
    AssetManager& asset_mgr = engine.get_asset_manager();
    Renderer& renderer = engine.get_renderer();
    asset_mgr.load_model(renderer, "assets/models/model.obj");

    //  Next screen
    load_demo_screen(game);
}
}
