cmake_minimum_required(VERSION 3.17)

project(spine)

set(SOURCE_FILES
    src/spine_extension.cpp
)

# To fix issues with Spine Cmake linker errors
file(GLOB INCLUDES "../extlibs/spine-runtimes/spine-cpp/spine-cpp/include/**/*.h")
file(GLOB SOURCES "../extlibs/spine-runtimes/spine-cpp/spine-cpp/src/**/*.cpp")

add_library(spine ${SOURCE_FILES} ${SOURCES} ${INCLUDES})

# To fix issues with Spine Cmake linker errors
target_include_directories(spine PUBLIC ../extlibs/spine-runtimes/spine-cpp/spine-cpp/include)
