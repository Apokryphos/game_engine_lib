#include "platform/glfw.hpp"

namespace engine
{
static double s_current = 0;
static double s_elapsed = 0;

//  ----------------------------------------------------------------------------
double get_elapsed_seconds() {
    return s_elapsed;
}

//  ----------------------------------------------------------------------------
double get_total_elapsed_seconds() {
    static double start = glfwGetTime();
    return s_current - start;
}

//  ----------------------------------------------------------------------------
void update_time() {
    static double current = glfwGetTime();
    static double last = current;

    current = glfwGetTime();
    s_current = current;

    s_elapsed = current - last;
    last = current;
}
}
