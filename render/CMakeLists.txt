cmake_minimum_required(VERSION 3.17)

project(render)

set(SOURCE_FILES
    src/render/renderer.cpp
)

add_library(render ${SOURCE_FILES})
target_include_directories(render PUBLIC include)

target_link_libraries(render
    common
    glm
    platform
    lodepng
)
