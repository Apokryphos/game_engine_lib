#include "platform/glfw.hpp"
#include "platform/window.hpp"
#include <cassert>

namespace platform
{
//  ----------------------------------------------------------------------------
Window::Window(GLFWwindow* window)
: m_window(window) {
    assert(m_window != nullptr);
}

//  ----------------------------------------------------------------------------
Window::~Window() {
}

//  ----------------------------------------------------------------------------
GLFWwindow* Window::get_glfw_window() {
    return m_window;
}

//  ----------------------------------------------------------------------------
bool Window::should_close() const {
    assert(m_window != nullptr);
    return glfwWindowShouldClose(m_window);
}
}
