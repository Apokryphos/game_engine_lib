#pragma once

struct GLFWwindow;

namespace platform
{
class Window
{
    GLFWwindow* m_window;

public:
    Window(GLFWwindow* window);
    ~Window();
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;
    GLFWwindow* get_glfw_window();
    bool should_close() const;
};
}
