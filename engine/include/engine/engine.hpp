#pragma once

#include <memory>
#include <string>

namespace assets
{
class AssetManager;
}

namespace input
{
class InputManager;
}

namespace platform
{
struct WindowOptions;
class Window;
}

namespace render
{
class Renderer;
}

namespace engine
{
class ScreenManager;
class UiStateManager;

class Engine
{
    using AssetManager = assets::AssetManager;
    using InputManager = input::InputManager;
    using RenderSystem = render::Renderer;
    using Window = platform::Window;
    using WindowOptions = platform::WindowOptions;

    std::unique_ptr<AssetManager> m_asset_mgr;
    std::unique_ptr<InputManager> m_input_mgr;
    std::unique_ptr<RenderSystem> m_render_sys;
    std::unique_ptr<ScreenManager> m_screen_mgr;
    std::unique_ptr<UiStateManager> m_ui_state_mgr;
    std::unique_ptr<Window> m_window;

public:
    Engine();
    ~Engine();
    Engine(const Engine&) = delete;
    Engine& operator=(const Engine&) = delete;
    AssetManager& get_asset_manager();
    InputManager& get_input_manager();
    RenderSystem& get_render_system();
    ScreenManager& get_screen_manager();
    UiStateManager& get_ui_state_manager();
    Window& get_window();
    bool initialize(
        const std::string& base_name,
        const WindowOptions& window_options,
        const size_t max_entities
    );
    void shutdown();
    void window_resized();
};
}
