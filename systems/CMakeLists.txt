cmake_minimum_required(VERSION 3.17)

project(systems)

set(SOURCE_FILES
    src/system_util.cpp
    src/debug_gui/name_system_debug_gui.cpp
)

add_library(systems ${SOURCE_FILES})
set_property(TARGET systems PROPERTY CXX_STANDARD 17)
target_include_directories(systems PUBLIC include)

target_link_libraries(systems
    engine
    filesystem
    input
)
