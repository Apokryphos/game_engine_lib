
cmake_minimum_required(VERSION 3.17)

project(render_gl)

set(SOURCE_FILES
    src/gl_renderer.cpp
)

add_library(render_gl ${SOURCE_FILES})
target_include_directories(render_gl PUBLIC include)

target_link_libraries(render_gl
    common
    glad
    glfw
    platform
    render
)
