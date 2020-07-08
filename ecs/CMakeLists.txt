cmake_minimum_required(VERSION 3.17)

project(ecs)

set(SOURCE_FILES
    src/ecs_root.cpp
    src/entity_manager.cpp
    src/entity_system_base.cpp
)

add_library(ecs ${SOURCE_FILES})
target_compile_definitions(ecs PUBLIC "$<$<CONFIG:DEBUG>:DEBUG>")
target_include_directories(ecs PUBLIC include)

target_link_libraries(ecs cereal common)
