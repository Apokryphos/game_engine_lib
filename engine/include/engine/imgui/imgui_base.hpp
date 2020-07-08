#pragma once

#include "render/render_api.hpp"

struct GLFWwindow;

namespace engine
{
//  Initializes ImGui (GLFW...renderer initialization is separate).
//  Render API is required because ImGui doesn't expose ImGui_ImplGlfw_Init,
//  only ImGui_ImplGlfw_InitForOpenGL and ImGui_ImplGlfw_InitForVulkan.
//  (Both of which, at this time, do nothing other than call ImGui_ImplGlfw_Init)
void imgui_init(GLFWwindow* window, render::RenderApi render_api);

void imgui_new_frame(render::RenderApi render_api);

//  Shutsdown ImGui (GLFW...renderer shutdown is separate).
void imgui_shutdown();
}
