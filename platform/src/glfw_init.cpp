#include "common/log.hpp"
#include "platform/glfw.hpp"

using namespace common;

namespace platform
{
//  ----------------------------------------------------------------------------
static void error_callback(int error, const char* description) {
    log_error("GLFW: %s", description);
}

//  ----------------------------------------------------------------------------
static void glfw_vulkan_window_hints() {
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
}

//  ----------------------------------------------------------------------------
static void glfw_gl_window_hints() {
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
}

//  ----------------------------------------------------------------------------
static GLFWwindow* glfw_create_window(
    int width,
    int height,
    const std::string& title
) {
    //  Create window
    GLFWwindow* window = glfwCreateWindow(
        width,
        height,
        title.c_str(),
        NULL,
        NULL
    );

    if (!window) {
        log_error("Failed to create GLFW window.");
        return nullptr;
    }

    return window;
}

//  ----------------------------------------------------------------------------
GLFWwindow* glfw_create_gl_window(
    int width,
    int height,
    const std::string& title
) {
    glfw_gl_window_hints();
    GLFWwindow* window = glfw_create_window(width, height, title);

    if (window != nullptr) {
        glfwMakeContextCurrent(window);
    }

    return window;
}

//  ----------------------------------------------------------------------------
GLFWwindow* glfw_create_vulkan_window(
    int width,
    int height,
    const std::string& title
) {
    glfw_vulkan_window_hints();
    return glfw_create_window(width, height, title);
}

//  ----------------------------------------------------------------------------
bool glfw_init() {
    log_debug("Initializing GLFW...");
    glfwSetErrorCallback(error_callback);
    if (!glfwInit()) {
        log_error("Failed to initialize GLFW.");
        return false;
    }

    return true;
}

//  ----------------------------------------------------------------------------
void glfw_shutdown(GLFWwindow* window) {
    log_debug("Shutting down GLFW...");
    glfwDestroyWindow(window);
    glfwTerminate();
}

//  ----------------------------------------------------------------------------
bool glad_init() {
    log_debug("Initializing glad...");
    if (!gladLoadGL()) {
        log_error("Failed to initialize glad.");
        return false;
    }

    return true;
}
}
