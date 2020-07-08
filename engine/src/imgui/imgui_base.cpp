#include "common/log.hpp"
#include "examples/imgui_impl_glfw.h"
#include "examples/imgui_impl_opengl3.h"
#include "examples/imgui_impl_vulkan.h"
#include "render/render_api.hpp"

using namespace common;
using namespace render;

struct GLFWwindow;

namespace engine
{
//  ----------------------------------------------------------------------------
void imgui_init(GLFWwindow* window, RenderApi render_api) {
    log_debug("Initializing ImGui...");

    //  Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    //  Use dark theme
    ImGui::StyleColorsDark();

    //  ImGui_ImplGlfw_Init isn't exposed so use the appropriate init function.
    bool result = false;
    if (render_api == RenderApi::OpenGl) {
        result = ImGui_ImplGlfw_InitForOpenGL(window, true);
    } else if (render_api == RenderApi::Vulkan) {
        result = ImGui_ImplGlfw_InitForVulkan(window, true);
    } else {
        throw std::runtime_error("Not implemented.");
    }

    if (!result) {
        throw std::runtime_error("Failed to initialize ImGui (GLFW).");
    }
}

//  ----------------------------------------------------------------------------
void imgui_new_frame(RenderApi render_api) {
    if (render_api == RenderApi::OpenGl) {
        ImGui_ImplOpenGL3_NewFrame();
    } else if (render_api == RenderApi::Vulkan) {
        ImGui_ImplVulkan_NewFrame();
    } else {
        throw std::runtime_error("Not implemented.");
    }

    ImGui_ImplGlfw_NewFrame();

    ImGui::NewFrame();
}

//  ----------------------------------------------------------------------------
void imgui_shutdown() {
    log_debug("Shutting down ImGui...");

    ImGui_ImplGlfw_Shutdown();

    ImGui::DestroyContext();
}
}
