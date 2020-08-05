cmake_minimum_required(VERSION 3.17)

project(assets)

set(SOURCE_FILES
    src/asset_manager.cpp
    src/asset_task_manager.cpp
)

add_library(assets ${SOURCE_FILES})

set_property(TARGET assets PROPERTY CXX_STANDARD 17)

target_compile_definitions(assets PUBLIC "$<$<CONFIG:DEBUG>:DEBUG>")

target_include_directories(assets PUBLIC include)

target_link_libraries(assets
    common
)

