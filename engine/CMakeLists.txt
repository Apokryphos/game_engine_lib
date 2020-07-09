cmake_minimum_required(VERSION 3.17)

project(engine)

set(SOURCE_FILES
    src/asset_manager.cpp
    src/base_systems/base_system_util.cpp
    src/base_systems/config_system.cpp
    src/base_systems/debug_gui_system.cpp
    src/base_systems/editor_system.cpp
    src/base_systems/profile_system.cpp
    src/debug_gui/config_system_debug_panel.cpp
    src/debug_gui/debug_gui_system_debug_panel.cpp
    src/debug_gui/debug_panel.cpp
    src/debug_gui/entity_info.cpp
    src/debug_gui/name_system_debug_panel.cpp
    src/debug_gui/profile_system_debug_panel.cpp
    src/editor/editor_panel.cpp
    src/editor/name_system_editor_panel.cpp
    src/engine.cpp
    src/game.cpp
    src/imgui/imgui_base.cpp
    src/screens/screen.cpp
    src/screens/screen_manager.cpp
    src/system_manager.cpp
    src/ui/ui_state.cpp
    src/ui/ui_state_manager.cpp
    src/time.cpp
)

add_library(engine ${SOURCE_FILES})

set_property(TARGET engine PROPERTY CXX_STANDARD 17)

target_compile_definitions(engine PUBLIC "$<$<CONFIG:DEBUG>:DEBUG>")

target_include_directories(engine PUBLIC include)

target_link_libraries(engine
    common
    ecs
    filesystem
    imgui
    input
    render
    render_gl
    render_vk
)

