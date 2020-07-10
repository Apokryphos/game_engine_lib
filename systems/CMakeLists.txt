cmake_minimum_required(VERSION 3.17)

project(systems)

set(SOURCE_FILES
    src/camera_system.cpp
    src/debug_gui/position_system_debug_panel.cpp
    src/editor/camera_system_editor_panel.cpp
    src/editor/position_system_editor_panel.cpp
    src/system_util.cpp
)

add_library(systems ${SOURCE_FILES})
set_property(TARGET systems PROPERTY CXX_STANDARD 17)
target_include_directories(systems PUBLIC include)

target_link_libraries(systems
    engine
    filesystem
    input
)
