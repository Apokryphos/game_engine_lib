#include "assets/asset_manager.hpp"
#include "common/log.hpp"
#include "demo/input/input_action_ids.hpp"
#include "demo/prefabs/entity_prefabs.hpp"
#include "demo/screens/demo_screen.hpp"
#include "demo/screens/init_screen.hpp"
#include "demo/systems/demo_system.hpp"
#include "ecs/ecs_root.hpp"
#include "engine/base_systems/config_system.hpp"
#include "engine/base_systems/editor_system.hpp"
#include "engine/base_systems/name_system.hpp"
#include "engine/base_systems/profile_system.hpp"
#include "engine/engine.hpp"
#include "engine/game.hpp"
#include "engine/screens/screen_manager.hpp"
#include "engine/system_manager.hpp"
#include "input/gamepad.hpp"
#include "input/input_manager.hpp"
#include "render/renderer.hpp"
#include "systems/camera_system.hpp"
#include "systems/debug_gui/position_system_debug_panel.hpp"
#include "systems/editor/camera_system_editor_panel.hpp"
#include "systems/editor/model_system_editor_panel.hpp"
#include "systems/editor/move_system_editor_panel.hpp"
#include "systems/editor/position_system_editor_panel.hpp"
#include "systems/billboard_system.hpp"
#include "systems/glyph_system.hpp"
#include "systems/model_system.hpp"
#include "systems/move_system.hpp"
#include "systems/position_system.hpp"
#include "systems/spine_system.hpp"
#include "systems/sprite_system.hpp"
#include "systems/system_util.hpp"
#include <set>

using namespace assets;
using namespace common;
using namespace demo;
using namespace ecs;
using namespace engine;
using namespace input;
using namespace render;
using namespace systems;

namespace glm
{
bool operator<(const glm::vec3& lhs, const glm::vec3& rhs) {
    if (lhs.y == rhs.y) {
        return lhs.x < rhs.x;
    }

    return lhs.y < rhs.y;
}
};

namespace demo
{
const int BILLBOARD_COUNT = 1000;
const int GLYPH_COUNT     = 32;//14516;
const int MODEL_COUNT     = 1000;
const int SPINE_COUNT     = 0;
const int SPRITE_COUNT    = 100;

//  ----------------------------------------------------------------------------
static void init_input_actions(InputManager& input_mgr) {
    log_debug("Initializing input actions...");

    InputActionSet& action_set = input_mgr.get_action_set();
    action_set.add_action(INPUT_ACTION_ID_TOGGLE_DEBUG_GUI, "Toggle Debug GUI", InputType::Digital, InputEventType::Pressed);
    action_set.add_action(INPUT_ACTION_ID_QUIT, "Quit", InputType::Digital, InputEventType::Pressed);
    action_set.add_action(INPUT_ACTION_ID_ACCEPT, "Accept", InputType::Digital, InputEventType::Pressed);
    action_set.add_action(INPUT_ACTION_ID_CANCEL, "Cancel", InputType::Digital, InputEventType::Pressed);
    action_set.add_action(INPUT_ACTION_ID_MOVE_FORWARD, "Move Forward", InputType::Analog, InputEventType::Down);
    action_set.add_action(INPUT_ACTION_ID_MOVE_BACKWARD, "Move Backward", InputType::Analog, InputEventType::Down);
    action_set.add_action(INPUT_ACTION_ID_MOVE_RIGHT, "Move Right", InputType::Analog, InputEventType::Down);
    action_set.add_action(INPUT_ACTION_ID_MOVE_LEFT, "Move Left", InputType::Analog, InputEventType::Down);
    action_set.add_action(INPUT_ACTION_ID_AIM_HORZ, "Aim X", InputType::Analog, InputEventType::Delta);
    action_set.add_action(INPUT_ACTION_ID_AIM_VERT, "Aim Y", InputType::Analog, InputEventType::Delta);
    action_set.add_action(INPUT_ACTION_ID_FIRE, "Fire", InputType::Digital, InputEventType::Delta);
    action_set.add_action(INPUT_ACTION_ID_TOGGLE_CAMERA, "Toggle Active Camera", InputType::Digital, InputEventType::Pressed);
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
    bind_map.bind_key(INPUT_ACTION_ID_MOVE_FORWARD, GLFW_KEY_UP);
    bind_map.bind_key(INPUT_ACTION_ID_MOVE_BACKWARD, GLFW_KEY_DOWN);
    bind_map.bind_key(INPUT_ACTION_ID_MOVE_RIGHT, GLFW_KEY_RIGHT);
    bind_map.bind_key(INPUT_ACTION_ID_MOVE_LEFT, GLFW_KEY_LEFT);
    bind_map.bind_key(INPUT_ACTION_ID_ROTATE_CW, GLFW_KEY_END);
    bind_map.bind_key(INPUT_ACTION_ID_ROTATE_CCW, GLFW_KEY_HOME);
    bind_map.bind_key(INPUT_ACTION_ID_TOGGLE_CAMERA, GLFW_KEY_TAB);
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
    bind_map.bind_mouse_wheel(INPUT_ACTION_ID_ZOOM_IN, AxisSign::Negative);
    bind_map.bind_mouse_wheel(INPUT_ACTION_ID_ZOOM_OUT, AxisSign::Positive);
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

    bind_map.bind_axis(INPUT_ACTION_ID_MOVE_FORWARD, GLFW_GAMEPAD_AXIS_LEFT_Y, AxisSign::Negative);
    bind_map.bind_axis(INPUT_ACTION_ID_MOVE_BACKWARD, GLFW_GAMEPAD_AXIS_LEFT_Y, AxisSign::Positive);
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
    DebugGuiSystem& debug_gui_system = get_debug_gui_system(sys_mgr);

    const size_t MAX_ENTITIES = InitScreen::MAX_ENTITIES;

    sys_mgr.add_system(std::make_unique<PositionSystem>(ecs, MAX_ENTITIES));
    debug_gui_system.add_gui(
        std::make_unique<PositionSystemDebugPanel>(get_position_system(sys_mgr))
    );

    sys_mgr.add_system(std::make_unique<BillboardSystem>(ecs, MAX_ENTITIES));
    sys_mgr.add_system(std::make_unique<CameraSystem>(ecs, MAX_ENTITIES));
    sys_mgr.add_system(std::make_unique<GlyphSystem>(ecs, MAX_ENTITIES));
    sys_mgr.add_system(std::make_unique<ModelSystem>(ecs, MAX_ENTITIES));
    sys_mgr.add_system(std::make_unique<MoveSystem>(ecs, MAX_ENTITIES));
    sys_mgr.add_system(std::make_unique<SpineSystem>(ecs, MAX_ENTITIES));
    sys_mgr.add_system(std::make_unique<SpriteSystem>(ecs, MAX_ENTITIES));

    //  Editors
    EditorSystem& editor_sys = get_editor_system(sys_mgr);
    editor_sys.add_panel(
        std::make_unique<CameraSystemEditorPanel>(get_camera_system(sys_mgr))
    );
    editor_sys.add_panel(
        std::make_unique<ModelSystemEditorPanel>(get_model_system(sys_mgr))
    );
    editor_sys.add_panel(
        std::make_unique<MoveSystemEditorPanel>(get_move_system(sys_mgr))
    );
    editor_sys.add_panel(
        std::make_unique<PositionSystemEditorPanel>(get_position_system(sys_mgr))
    );
}

//  ----------------------------------------------------------------------------
static void init_assets(Game& game) {
    Engine& engine = game.get_engine();
    AssetManager& asset_mgr = engine.get_asset_manager();

    //  Load models
    asset_mgr.load_model("assets/models/model2.glb");
    asset_mgr.load_model("assets/models/model.obj");

    //  Load textures
    // asset_mgr.load_texture("assets/textures/missing.png");
    asset_mgr.load_texture("assets/textures/model.png");
    asset_mgr.load_texture("assets/textures/model2.png");
    asset_mgr.load_texture("assets/textures/model3.png");

    //  Pixel sprites
    TextureCreateArgs tex_args{};
    tex_args.mag_filter = TextureFilter::Nearest;
    tex_args.min_filter = TextureFilter::Nearest;
    tex_args.mipmaps = false;

    asset_mgr.load_texture("assets/textures/sprite.png", tex_args);
    asset_mgr.load_texture("assets/textures/billboard.png", tex_args);

    //  Load Spine assets
    asset_mgr.load_spine("assets/spine/spineboy/spineboy", tex_args);

    //  Load glyph textures
    asset_mgr.load_texture("assets/textures/cp437_20x20.png", tex_args);
    for (int n = 0; n < 256; ++n) {
        const std::string filename =
            "assets/textures/cp437_20x20/cp437_20x20_" +
            std::to_string(n) +
            ".png";
        asset_mgr.load_texture(filename, tex_args);
    }
}

//  ----------------------------------------------------------------------------
static void init_billboards(Game& game) {
    EcsRoot& ecs = game.get_ecs_root();
    SystemManager& sys_mgr = game.get_system_manager();

    std::uniform_int_distribution<int> x_dist(-50, 50);
    std::uniform_int_distribution<int> y_dist(-50, 50);

    Random& random = game.get_random();

    std::set<glm::vec3> position_set;
    while (position_set.size() < BILLBOARD_COUNT) {
        const glm::vec3 position = {
            x_dist(random.get_rng()),
            y_dist(random.get_rng()),
            0.0f
        };

        position_set.insert(position);
    }

    std::vector<glm::vec3> positions;
    std::copy(position_set.begin(), position_set.end(), std::back_inserter(positions));

    BillboardSystem& billboard_sys = get_billboard_system(sys_mgr);
    NameSystem& name_sys = get_name_system(sys_mgr);
    PositionSystem& pos_sys = get_position_system(sys_mgr);

    const int entity_count = positions.size();
    for (int n = 0; n < entity_count; ++n) {
        Entity entity = ecs.create_entity();

        const std::string name = "entity_" + std::to_string(entity.id);

        name_sys.add_component(entity);
        const auto name_cmpnt = name_sys.get_component(entity);
        name_sys.set_name(name_cmpnt, name);

        const glm::vec3 position = positions[n];

        add_position_component(entity, pos_sys, position);

        add_billboard_component(
            entity,
            billboard_sys,
            5,
            glm::vec2(1.0f)
        );
    }
}

//  ----------------------------------------------------------------------------
static void init_glyphs(Game& game) {
    EcsRoot& ecs = game.get_ecs_root();
    SystemManager& sys_mgr = game.get_system_manager();

    std::uniform_int_distribution<int> glyph_dist(0, 256);
    std::uniform_int_distribution<int> x_dist(0, 191);
    std::uniform_int_distribution<int> y_dist(0, 76);
    std::uniform_real_distribution<float> color_dist(0, 1.0);

    Random& random = game.get_random();

    AssetManager& asset_mgr = game.get_engine().get_asset_manager();

    const int glyph_set_texture_id = asset_mgr.get_texture_id("assets/textures/cp437_20x20.png");
    const int glyph_set_width = 20;
    const int glyph_set_height = 20;

    std::set<glm::vec3> position_set;
    while (position_set.size() < GLYPH_COUNT) {
        const glm::vec3 position = {
            glyph_set_width * x_dist(random.get_rng()),
            glyph_set_height * y_dist(random.get_rng()),
            0.0f
        };

        position_set.insert(position);
    }

    std::vector<glm::vec3> positions;
    std::copy(position_set.begin(), position_set.end(), std::back_inserter(positions));

    GlyphSystem& glyph_sys = get_glyph_system(sys_mgr);
    glyph_sys.add_glyph_set(glyph_set_texture_id, glyph_set_width, glyph_set_height);

    NameSystem& name_sys = get_name_system(sys_mgr);
    PositionSystem& pos_sys = get_position_system(sys_mgr);

    const int entity_count = positions.size();
    for (int n = 0; n < entity_count; ++n) {
        Entity entity = ecs.create_entity();

        const std::string name = "entity_" + std::to_string(entity.id);

        name_sys.add_component(entity);
        const auto name_cmpnt = name_sys.get_component(entity);
        name_sys.set_name(name_cmpnt, name);

        const glm::vec3 position = positions[n];

        add_position_component(entity, pos_sys, position);

        const uint16_t glyph = glyph_dist(random.get_rng());

        const glm::vec4 bg_color {
            color_dist(random.get_rng()),
            color_dist(random.get_rng()),
            color_dist(random.get_rng()),
            1.0f
        };

        const glm::vec4 fg_color {
            color_dist(random.get_rng()),
            color_dist(random.get_rng()),
            color_dist(random.get_rng()),
            1.0f
        };

        add_glyph_component(entity, glyph_sys, 0, glyph, bg_color, fg_color);
    }
}

//  ----------------------------------------------------------------------------
static void init_models(Game& game) {
    EcsRoot& ecs = game.get_ecs_root();
    SystemManager& sys_mgr = game.get_system_manager();

    std::uniform_int_distribution<int> model_id_dist(0, 1);
    std::uniform_int_distribution<int> texture_id_dist(1, 3);
    std::uniform_int_distribution<int> position_dist(-100, 100);

    Random& random = game.get_random();

    std::set<glm::vec3> position_set;
    while (position_set.size() < MODEL_COUNT) {
        const glm::vec3 position = {
            position_dist(random.get_rng()) * 2,
            position_dist(random.get_rng()) * 2,
            0.0f
        };

        position_set.insert(position);
    }

    std::vector<glm::vec3> positions;
    std::copy(position_set.begin(), position_set.end(), std::back_inserter(positions));

    PositionSystem& pos_sys = get_position_system(sys_mgr);
    ModelSystem& model_sys = get_model_system(sys_mgr);
    NameSystem& name_sys = get_name_system(sys_mgr);

    const int entity_count = positions.size();
    for (int n = 0; n < entity_count; ++n) {
        Entity entity = ecs.create_entity();

        const std::string name = "entity_" + std::to_string(entity.id);

        name_sys.add_component(entity);
        const auto name_cmpnt = name_sys.get_component(entity);
        name_sys.set_name(name_cmpnt, name);

        const glm::vec3 position = positions[n];

        add_position_component(entity, pos_sys, position);

        add_model_component(
            entity,
            model_sys,
            model_id_dist(random.get_rng()),
            texture_id_dist(random.get_rng())
        );
    }
}

//  ----------------------------------------------------------------------------
static void init_spines(Game& game) {
    EcsRoot& ecs = game.get_ecs_root();
    SystemManager& sys_mgr = game.get_system_manager();

    std::uniform_int_distribution<int> x_dist(800, 800);
    std::uniform_int_distribution<int> y_dist(1200, 1200);

    Random& random = game.get_random();

    std::set<glm::vec3> position_set;
    while (position_set.size() < SPINE_COUNT) {
        const glm::vec3 position = {
            x_dist(random.get_rng()),
            y_dist(random.get_rng()),
            0.0f
        };

        position_set.insert(position);
    }

    std::vector<glm::vec3> positions;
    std::copy(position_set.begin(), position_set.end(), std::back_inserter(positions));

    NameSystem& name_sys = get_name_system(sys_mgr);
    PositionSystem& pos_sys = get_position_system(sys_mgr);
    SpineSystem& spine_sys = get_spine_system(sys_mgr);

    const int entity_count = positions.size();
    for (int n = 0; n < entity_count; ++n) {
        Entity entity = ecs.create_entity();

        const std::string name = "entity_" + std::to_string(entity.id);

        name_sys.add_component(entity);
        const auto name_cmpnt = name_sys.get_component(entity);
        name_sys.set_name(name_cmpnt, name);

        const glm::vec3 position = positions[n];
        add_position_component(entity, pos_sys, position);

        add_spine_component(entity, spine_sys, 0);
    }
}

//  ----------------------------------------------------------------------------
static void init_sprites(Game& game) {
    EcsRoot& ecs = game.get_ecs_root();
    SystemManager& sys_mgr = game.get_system_manager();

    std::uniform_real_distribution<float> size_dist(0.5f, 1.0f);
    std::uniform_int_distribution<int> x_dist(0, 3000);
    std::uniform_int_distribution<int> y_dist(0, 2000);

    Random& random = game.get_random();

    std::set<glm::vec3> position_set;
    while (position_set.size() < SPRITE_COUNT) {
        const glm::vec3 position = {
            x_dist(random.get_rng()),
            y_dist(random.get_rng()),
            0.0f
        };

        position_set.insert(position);
    }

    std::vector<glm::vec3> positions;
    std::copy(position_set.begin(), position_set.end(), std::back_inserter(positions));

    NameSystem& name_sys = get_name_system(sys_mgr);
    PositionSystem& pos_sys = get_position_system(sys_mgr);
    SpriteSystem& sprite_sys = get_sprite_system(sys_mgr);

    const int entity_count = positions.size();
    for (int n = 0; n < entity_count; ++n) {
        Entity entity = ecs.create_entity();

        const std::string name = "entity_" + std::to_string(entity.id);

        name_sys.add_component(entity);
        const auto name_cmpnt = name_sys.get_component(entity);
        name_sys.set_name(name_cmpnt, name);

        const glm::vec3 position = positions[n];

        add_position_component(entity, pos_sys, position);

        add_sprite_component(
            entity,
            sprite_sys,
            4,
            glm::vec2(
                102 * size_dist(random.get_rng()),
                100 * size_dist(random.get_rng())
            )
        );
    }
}

//  ----------------------------------------------------------------------------
static void init_entities(Game& game) {
    EcsRoot& ecs = game.get_ecs_root();
    SystemManager& sys_mgr = game.get_system_manager();

    NameSystem& name_sys = get_name_system(sys_mgr);
    PositionSystem& pos_sys = get_position_system(sys_mgr);

    //  Camera
    Entity camera = ecs.create_entity();
    make_camera(game, camera, "camera", glm::vec3(1.0f), 10.0f);

    //  2D camera
    Entity ortho_camera = ecs.create_entity();
    make_camera(
        game,
        ortho_camera,
        "ortho_camera",
        glm::vec3(0.0f),
        64.0f,
        true
    );

    CameraSystem& camera_sys = get_camera_system(sys_mgr);
    activate_camera(camera, camera_sys);

    init_billboards(game);
    init_glyphs(game);
    init_models(game);
    init_spines(game);
    init_sprites(game);
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
    //  Initialize input
    init_input(game);

    //  Initialize systems
    init_systems(game);

    //  Initialize ECS systems
    init_ecs_systems(game);

    init_assets(game);

    //  Initialize entities
    init_entities(game);

    //  Next screen
    load_demo_screen(game);
}
}
