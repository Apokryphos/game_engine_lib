#pragma once

#include "platform/glfw.hpp"
#include <string>

namespace platform
{
bool glad_init();

GLFWwindow* glfw_create_gl_window(
    int width,
    int height,
    const std::string& title
);

GLFWwindow* glfw_create_vulkan_window(
    int width,
    int height,
    const std::string& title
);

bool glfw_init();
void glfw_shutdown(GLFWwindow* window);
}
