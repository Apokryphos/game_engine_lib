cmake_minimum_required(VERSION 3.17)

project(render_vk)

set(SOURCE_FILES
    src/buffer.cpp
    src/command_buffer.cpp
    src/command_pool.cpp
    src/debug_gui/vulkan_debug_panel.cpp
    src/debug_utils.cpp
    src/depth.cpp
    src/descriptor_pool.cpp
    src/descriptor_set_layout.cpp
    src/descriptor_sets.cpp
    src/devices.cpp
    src/dynamic_uniform_buffer.cpp
    src/framebuffers.cpp
    src/image_view.cpp
    src/imgui/imgui_vk.cpp
    src/index_buffer.cpp
    src/instance.cpp
    src/graphics_pipeline.cpp
    src/frame_sync.cpp
    src/mesh.cpp
    src/model_manager.cpp
    src/queue_family.cpp
    # src/render_job_manager.cpp
    src/render_pass.cpp
    # src/render_tasks/draw_models_task.cpp
    src/render_tasks/load_model_task.cpp
    src/render_tasks/load_texture_task.cpp
    # src/renderers/vulkan_model_renderer.cpp
    src/texture.cpp
    src/vertex_buffer.cpp
    src/vulkan_model.cpp
    src/vulkan_queue.cpp
    # src/vulkan_renderer.cpp
    src/vulkan_render_system.cpp
    src/vulkan_swapchain.cpp
)

add_library(render_vk ${SOURCE_FILES})

set_property(TARGET render_vk PROPERTY CXX_STANDARD 17)

target_compile_definitions(render_vk PUBLIC "$<$<CONFIG:DEBUG>:DEBUG>")

# Enable experimental GLM extensions
target_compile_definitions(render_vk PUBLIC GLM_ENABLE_EXPERIMENTAL)
# Use aligned types by default for vector types
target_compile_definitions(render_vk PUBLIC GLM_FORCE_DEFAULT_ALIGNED_GENTYPES)
# Force GLM perspective matrix to Vulkan range of [0.0...1.0]
target_compile_definitions(render_vk PUBLIC GLM_FORCE_DEPTH_ZERO_TO_ONE)
# Force all angles to radians
target_compile_definitions(render_vk PUBLIC GLM_FORCE_RADIANS)
# Remove all Vulkan prototypes
target_compile_definitions(render_vk PUBLIC VK_NO_PROTOTYPES)
# Include tinyobjloader function bodies
target_compile_definitions(render_vk PUBLIC TINYOBJLOADER_IMPLEMENTATION)

target_include_directories(render_vk PUBLIC include)

target_link_libraries(render_vk
    common
    glfw
    imgui
    platform
    render
    tinyobjloader
    volk
    vulkan
)
