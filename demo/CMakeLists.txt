cmake_minimum_required(VERSION 3.17)

project(demo)

set(SOURCE_FILES
    src/prefabs/entity_prefabs.cpp
    src/ui/demo_state.cpp
    src/screens/demo_screen.cpp
    src/screens/init_screen.cpp
    src/systems/demo_system.cpp
    src/main.cpp
)

add_executable(demo ${SOURCE_FILES})
set_property(TARGET demo PROPERTY CXX_STANDARD 17)
target_include_directories(demo PRIVATE include)

target_link_libraries(demo
    engine
    render_vk
    systems
)
