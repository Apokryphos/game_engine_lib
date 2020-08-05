# cereal
project(cereal)
add_library(cereal INTERFACE)
target_include_directories(cereal INTERFACE cereal/include)

# glad
project(glad)
add_library(glad glad/src/glad.c)
target_include_directories(glad PUBLIC glad/include)

# GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INCLUDE_VULKAN ON CACHE BOOL "" FORCE)

add_subdirectory(glfw)

# GLM
add_subdirectory(glm)

# lodepng
project(lodepng)
add_library(lodepng lodepng/lodepng.cpp)
target_include_directories(lodepng PUBLIC lodepng)

# spine-cpp
# Linking to spine-cpp gives linker errors
# add_subdirectory(spine-runtimes/spine-cpp)

# tinyglft
project(tinygltf)
add_library(tinygltf INTERFACE)
target_include_directories(tinygltf INTERFACE tinygltf)

# tinyobjloader
project(tinyobjloader)
add_library(tinyobjloader INTERFACE)
target_include_directories(tinyobjloader INTERFACE tinyobjloader)

# volk
add_subdirectory(volk)

# vulkan
project(vulkan)
add_library(vulkan INTERFACE)
target_include_directories(vulkan INTERFACE vulkan_1.2.141.2/x86_64/include)

# imgui
# volk requires that its own <volk.h> header be included and not <vulkan/vulkan.h>.
# VK_NO_PROTOTYPES has to be defined or ImGui will cause segmentation faults
# (e.g. in ImGui_ImplVulkan_Init).
# ImGui may require at least one modification:
# Replace <vulkan/vulkan.h> include with <volk.h> in imgui_impl_vulkan.cpp.
project(imgui)
set(IMGUI_SOURCE_FILES
    imgui/imgui.cpp
    imgui/imgui_demo.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_widgets.cpp
    imgui/examples/imgui_impl_glfw.cpp
    imgui/examples/imgui_impl_opengl3.cpp
    imgui/examples/imgui_impl_vulkan.cpp
)
add_library(imgui ${IMGUI_SOURCE_FILES})
# Remove all Vulkan prototypes
target_compile_definitions(imgui PUBLIC VK_NO_PROTOTYPES)
# Use glad as OpenGL loader
target_compile_definitions(imgui PUBLIC IMGUI_IMPL_OPENGL_LOADER_GLAD)
target_include_directories(imgui PUBLIC imgui)
target_link_libraries(imgui glad volk)
