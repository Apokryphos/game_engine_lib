#include "common/log.hpp"
#include "engine/asset_manager.hpp"
#include "engine/engine.hpp"
#include "engine/imgui/imgui_base.hpp"
#include "engine/screens/screen_manager.hpp"
#include "engine/ui/ui_state_manager.hpp"
#include "input/input_manager.hpp"
#include "platform/glfw_init.hpp"
#include "platform/window.hpp"
#include "platform/window_options.hpp"
// #include "render_gl/gl_renderer.hpp"
#include "render_vk/vulkan_render_system.hpp"
#include <cassert>

using namespace common;
using namespace input;
using namespace platform;
using namespace render;
// using namespace render_gl;
using namespace render_vk;

namespace engine
{
//  ----------------------------------------------------------------------------
static void framebuffer_size_callback(
    GLFWwindow* window,
    int width,
    int height
) {
    auto engine = reinterpret_cast<Engine*>(glfwGetWindowUserPointer(window));
    engine->window_resized();
}

//  ----------------------------------------------------------------------------
Engine::Engine()
: m_input_mgr(nullptr),
  m_renderer(nullptr),
  m_screen_mgr(std::make_unique<ScreenManager>()),
  m_ui_state_mgr(std::make_unique<UiStateManager>()),
  m_window(nullptr) {
}

//  ----------------------------------------------------------------------------
Engine::~Engine() {
}

//  ----------------------------------------------------------------------------
AssetManager& Engine::get_asset_manager() {
    assert(m_asset_mgr != nullptr);
    return *m_asset_mgr;
}

//  ----------------------------------------------------------------------------
InputManager& Engine::get_input_manager() {
    assert(m_input_mgr != nullptr);
    return *m_input_mgr;
}

//  ----------------------------------------------------------------------------
Renderer& Engine::get_renderer() {
    assert(m_renderer != nullptr);
    return *m_renderer;
}

//  ----------------------------------------------------------------------------
ScreenManager& Engine::get_screen_manager() {
    assert(m_screen_mgr != nullptr);;
    return *m_screen_mgr;
}

//  ----------------------------------------------------------------------------
UiStateManager& Engine::get_ui_state_manager() {
    assert(m_ui_state_mgr != nullptr);;
    return *m_ui_state_mgr;
}

//  ----------------------------------------------------------------------------
Window& Engine::get_window() {
    assert(m_window != nullptr);
    return *m_window;
}

//  ----------------------------------------------------------------------------
bool Engine::initialize(
    const std::string& base_name,
    const WindowOptions& window_options
) {
    log_debug("Initializing engine...");

    //  Check that base name is valid
    if (base_name.empty()) {
        log_debug("Base name cannot be empty.");
        return false;
    }

    const RenderApi render_api = RenderApi::Vulkan;

    //  Initialize GLFW
    if (!glfw_init()) {
        return false;
    }

    //  Create GLFW window
    GLFWwindow* glfw_window = NULL;
    if (render_api == RenderApi::OpenGl) {
        glfw_window = glfw_create_gl_window(
            window_options.width,
            window_options.height,
            window_options.title
        );
    } else if (render_api == RenderApi::Vulkan) {
        glfw_window = glfw_create_vulkan_window(
            window_options.width,
            window_options.height,
            window_options.title
        );
    } else {
        throw std::runtime_error("Not implemented.");
    }

    //  Check that GLFW window is valid
    if (glfw_window == nullptr) {
        return false;
    }

    //  Window
    m_window = std::make_unique<Window>(glfw_window);

    //  Input manager
    m_input_mgr = std::make_unique<InputManager>(glfw_window);

    //  Initialize ImGui (GLFW) before renderer
    //  Renderer will setup ImGui rendering specifics
    imgui_init(glfw_window, render_api);

    //  Create renderer for API
    if (render_api == RenderApi::OpenGl) {
        throw std::runtime_error("Not implemented.");
        // m_renderer = std::make_unique<GlRenderer>();
    } else if (render_api == RenderApi::Vulkan) {
        m_renderer = std::make_unique<VulkanRenderSystem>();
    } else {
        throw std::runtime_error("Not implemented.");
    }

    //  Initialize renderer
    if (!m_renderer->initialize(glfw_window)) {
        log_error("Failed to initialize renderer.");
        glfw_shutdown(glfw_window);
        return false;
    }

    //  Window resize callback
    glfwSetWindowUserPointer(glfw_window, this);
    glfwSetFramebufferSizeCallback(glfw_window, framebuffer_size_callback);

    m_asset_mgr = std::make_unique<AssetManager>();

    return true;
}

//  ----------------------------------------------------------------------------
void Engine::shutdown() {
    if (m_renderer != nullptr) {
        m_renderer->shutdown();
    }

    //  Shutdown ImGui after renderer (GLFW)
    //  Renderer will destroy ImGui rendering specifics
    imgui_shutdown();

    if (m_window != nullptr) {
        glfw_shutdown(m_window->get_glfw_window());
        m_window = nullptr;
    }
}

//  ----------------------------------------------------------------------------
void Engine::window_resized() {
    assert(m_window != nullptr);
    assert(m_renderer != nullptr);
    m_renderer->resize();
}
}
