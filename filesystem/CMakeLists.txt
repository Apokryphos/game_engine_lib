cmake_minimum_required(VERSION 3.17)

project(filesystem)

set(SOURCE_FILES
    src/paths.cpp
)

add_library(filesystem ${SOURCE_FILES})
set_property(TARGET filesystem PROPERTY CXX_STANDARD 17)
target_include_directories(filesystem PUBLIC include)

target_link_libraries(filesystem
    common
    stdc++fs
)
