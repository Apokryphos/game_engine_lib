cmake_minimum_required(VERSION 3.17)

project(platform)

set(SOURCE_FILES
    src/glfw_init.cpp
    src/window.cpp
)

add_library(platform ${SOURCE_FILES})
target_include_directories(platform PUBLIC include)

target_link_libraries(platform
    common
    glad
    glfw
)
