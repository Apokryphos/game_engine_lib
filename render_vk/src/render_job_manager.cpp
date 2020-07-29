#include "render_vk/depth.hpp"
#include "render_vk/render_job_manager.hpp"
#include "render_vk/vulkan_queue.hpp"
#include "render_vk/vulkan_swapchain.hpp"

using namespace common;

namespace render_vk
{
//  ----------------------------------------------------------------------------
RenderJobManager::RenderJobManager(const uint32_t max_frames_in_flight)
: m_max_frames_in_flight(max_frames_in_flight) {
    initialize_tasks();
}

//  ----------------------------------------------------------------------------
RenderJobManager::~RenderJobManager() {
    shutdown();
}

//  ----------------------------------------------------------------------------
void RenderJobManager::start_threads(
    VkPhysicalDevice physical_device,
    VkDevice device,
    VulkanQueue& graphics_queue,
    VulkanSwapchain& swapchain,
    DepthImage& depth_image
) {
    // create_secondary_command_objects(
    //     m_device,
    //     m_physical_device,
    //     m_graphics_queue,
    //     m_swapchain,
    //     m_depth_image,
    //     frame_name,
    //     frame.command
    // );

    // create_descriptor_objects(
    //     m_device,
    //     m_descriptor_set_layouts,
    //     textures,
    //     m_frame_uniform,
    //     m_object_uniform,
    //     frame_name,
    //     frame.descriptor
    // );
}
}
