cmake_minimum_required(VERSION 3.17)

project(common)

set(SOURCE_FILES
    src/alloc.cpp
    src/ini_config.cpp
    src/thread_manager.cpp
)

add_library(common ${SOURCE_FILES})
target_compile_definitions(common PUBLIC "$<$<CONFIG:DEBUG>:DEBUG>")
target_include_directories(common PUBLIC include)
