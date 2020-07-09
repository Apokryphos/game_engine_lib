cmake_minimum_required(VERSION 3.17)

project(input)

set(SOURCE_FILES
    src/input/gamepad.cpp
    src/input/input_device.cpp
    src/input/input_manager.cpp
    src/input/keyboard.cpp
    src/input/mouse.cpp
    src/input/strings/input_action_strings.cpp
    src/input/strings/input_strings.cpp
)

add_library(input ${SOURCE_FILES})
set_property(TARGET input PROPERTY CXX_STANDARD 17)
target_include_directories(input PUBLIC include)

target_link_libraries(input
    common
    imgui
    platform
)
