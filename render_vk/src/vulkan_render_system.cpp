#include "common/log.hpp"
#include "common/stopwatch.hpp"
#include "platform/glfw_init.hpp"
#include "render_vk/command_buffer.hpp"
#include "render_vk/command_pool.hpp"
#include "render_vk/debug_utils.hpp"
#include "render_vk/depth.hpp"
#include "render_vk/descriptor_set_layout.hpp"
#include "render_vk/devices.hpp"
#include "render_vk/framebuffers.hpp"
#include "render_vk/imgui/imgui_vk.hpp"
#include "render_vk/instance.hpp"
#include "render_vk/mesh.hpp"
#include "render_vk/model_manager.hpp"
#include "render_vk/render_pass.hpp"
#include "render_vk/render_tasks/task_update_uniforms.hpp"
#include "render_vk/texture.hpp"
#include "render_vk/texture_manager.hpp"
#include "render_vk/vulkan.hpp"
#include "render_vk/render_task_manager.hpp"
#include "render_vk/renderers/billboard_renderer.hpp"
#include "render_vk/renderers/model_renderer.hpp"
#include "render_vk/renderers/sprite_renderer.hpp"
#include "render_vk/vulkan_render_system.hpp"
#include "render_vk/vulkan_model.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <cassert>
#include <thread>

using namespace common;
using namespace render;

namespace render_vk
{
static const uint32_t MAX_OBJECTS = 10000;
static Stopwatch STOPWATCH;

//  ----------------------------------------------------------------------------
static void create_sync_objects(
    VkDevice device,
    const std::string& name_prefix,
    FrameSyncObjects& sync
) {
    VkSemaphoreCreateInfo semaphore_info{};
    semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    if (vkCreateSemaphore(device, &semaphore_info, nullptr, &sync.image_acquired) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create semaphore");
    }

    set_debug_name(
        device,
        VK_OBJECT_TYPE_SEMAPHORE,
        sync.image_acquired,
        (name_prefix + "_image_acquired_semaphore").c_str()
    );

    if (vkCreateSemaphore(device, &semaphore_info, nullptr, &sync.present_ready) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create semaphore");
    }

    set_debug_name(
        device,
        VK_OBJECT_TYPE_SEMAPHORE,
        sync.present_ready,
        (name_prefix + "_present_ready_semaphore").c_str()
    );

    VkFenceCreateInfo fence_info{};
    fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    if (vkCreateFence(device, &fence_info, nullptr, &sync.frame_complete) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create fence.");
    }

    set_debug_name(
        device,
        VK_OBJECT_TYPE_FENCE,
        sync.frame_complete,
        (name_prefix + "_frame_complete_fence").c_str()
    );
}

//  ----------------------------------------------------------------------------
static void create_primary_command_objects(
    VkDevice device,
    VkPhysicalDevice physical_device,
    FrameCommandObjects& frame_command
) {
    create_command_pool(
        device,
        physical_device,
        frame_command.pool,
        "primary_command_pool"
    );

    create_primary_command_buffer(
        device,
        frame_command.pool,
        frame_command.buffer,
        "primary_command_buffer"
    );
}

//  ----------------------------------------------------------------------------
VulkanRenderSystem::VulkanRenderSystem()
: Renderer(RenderApi::Vulkan),
  m_frames(m_frame_count),
  m_glfw_window(nullptr),
  m_object_uniform(MAX_OBJECTS) {
    assert(m_frames.size() > 0);
}

//  ----------------------------------------------------------------------------
VulkanRenderSystem::~VulkanRenderSystem() {
}

//  ----------------------------------------------------------------------------
void VulkanRenderSystem::begin_frame() {
    STOPWATCH.start("begin_frame");

    //  Check that texture transfers are finished
    // if (m_texture_mgr->has_pending_textures()) {
    //     m_frame_status = FrameStatus::Discarded;
    //     m_render_task_mgr->begin_frame(m_current_frame, true);
    //     return;
    // }

    const std::vector<Texture>& textures = m_texture_mgr->get_textures();

    m_frame_status = FrameStatus::None;

    Frame& frame = m_frames.at(m_current_frame);

    //  Wait until resources for this frame are safe to use
    vkWaitForFences(
        m_device,
        1,
        &frame.sync.frame_complete,
        VK_TRUE,
        UINT64_MAX
    );

    //  Get next presentable swapchain image index
    VkResult result = vkAcquireNextImageKHR(
        m_device,
        m_swapchain.swapchain,
        UINT64_MAX,
        frame.sync.image_acquired,
        VK_NULL_HANDLE,
        &m_image_index
    );

    //  Check if swapchain needs to be recreated
    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        //  Surface changed and swapchain is no longer compatible
        recreate_swapchain();
        m_frame_status = FrameStatus::Discarded;
        m_render_task_mgr->begin_frame(m_current_frame, true);
        return;
    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("Failed to acquire swapchain image.");
    }

    m_frame_status = FrameStatus::Busy;

    m_render_task_mgr->begin_frame(m_current_frame, false);

    STOPWATCH.stop("begin_frame");
}

//  ----------------------------------------------------------------------------
bool VulkanRenderSystem::check_render_tasks_complete() {
    if (m_frame_status != FrameStatus::Busy) {
        //  Worker threads are not processing tasks this frame
        return true;
    }

    if (m_render_task_mgr->check_tasks_complete()) {
        //  Worker threads have completed all tasks for this frame
        m_frame_status = FrameStatus::Ready;
        return true;
    }

    //  Worker threads are still processing tasks for this frame
    return false;
}

//  ----------------------------------------------------------------------------
void VulkanRenderSystem::create_frame_resources() {
    //  Create frame resources
    m_frames.resize(m_frame_count);
    for (auto n = 0; n < m_frame_count; ++n) {
        const std::string frame_name = "frame" + std::to_string(n);

        create_sync_objects(m_device, frame_name, m_frames[n].sync);
        create_primary_command_objects(
            m_device,
            m_physical_device,
            m_frames[n].command
        );
    }
}

//  ----------------------------------------------------------------------------
void VulkanRenderSystem::create_swapchain_objects() {
    //  Get window size
    int width;
    int height;
    glfwGetWindowSize(m_glfw_window, &width, &height);

    //  Create swap chain
    render_vk::create_swapchain(
        m_device,
        m_physical_device,
        m_surface,
        width,
        height,
        m_swapchain
    );

    //  Create render pass
    create_render_pass(m_device, m_physical_device, m_swapchain, m_render_pass);
}

//  ----------------------------------------------------------------------------
void VulkanRenderSystem::create_swapchain_dependents() {
    create_command_pool(
        m_device,
        m_physical_device,
        m_resource_command_pool,
        "resource_command_pool"
    );

    m_billboard_renderer->create_objects(
        m_device,
        m_swapchain,
        m_render_pass,
        m_descriptor_set_layouts
    );

    m_model_renderer->create_objects(
        m_device,
        m_swapchain,
        m_render_pass,
        m_descriptor_set_layouts
    );

    m_sprite_renderer->create_objects(
        m_device,
        m_swapchain,
        m_render_pass,
        m_descriptor_set_layouts
    );

    create_depth_resources(
        m_physical_device,
        m_device,
        m_graphics_queue,
        m_resource_command_pool,
        m_swapchain,
        m_depth_image
    );

    create_framebuffers(
        m_device,
        m_render_pass,
        m_depth_image.view,
        m_swapchain
    );

    imgui_vulkan_init(
        m_instance,
        m_physical_device,
        m_device,
        m_graphics_queue.get_queue(),
        m_swapchain,
        m_render_pass,
        m_resource_command_pool
    );
}

//  ----------------------------------------------------------------------------
void VulkanRenderSystem::destroy_frame_resources() {
    //  Destroy main thread frame objects
    for (Frame& frame : m_frames) {
        vkDestroySemaphore(m_device, frame.sync.image_acquired, nullptr);
        vkDestroySemaphore(m_device, frame.sync.present_ready, nullptr);
        vkDestroyFence(m_device, frame.sync.frame_complete, nullptr);
        vkDestroyCommandPool(m_device, frame.command.pool, nullptr);
    }
    m_frames.clear();
}

//  ----------------------------------------------------------------------------
void VulkanRenderSystem::destroy_swapchain() {
    vkDestroyCommandPool(m_device, m_resource_command_pool, nullptr);

    //  Depth testing
    vkDestroyImageView(m_device, m_depth_image.view, nullptr);
    vkDestroyImage(m_device, m_depth_image.image, nullptr);
    vkFreeMemory(m_device, m_depth_image.memory, nullptr);

    //  Destroy framebuffers before respective images views and render pass
    for (auto framebuffer : m_swapchain.framebuffers) {
        vkDestroyFramebuffer(m_device, framebuffer, nullptr);
    }

    m_billboard_renderer->destroy_objects();
    m_model_renderer->destroy_objects();
    m_sprite_renderer->destroy_objects();

    imgui_vulkan_cleanup_swapchain(m_device);

    //  Destroy swapchain image views
    for (size_t n = 0; n < m_swapchain.image_views.size(); ++n) {
        vkDestroyImageView(m_device, m_swapchain.image_views[n], nullptr);
    }

    vkDestroyRenderPass(m_device, m_render_pass, nullptr);

    //  Destroy swapchain
    vkDestroySwapchainKHR(m_device, m_swapchain.swapchain, nullptr);
}

//  ----------------------------------------------------------------------------
void VulkanRenderSystem::draw_billboards(
    std::vector<render::SpriteBatch>& batches
) {
    m_render_task_mgr->draw_billboards(*m_billboard_renderer, batches);
}

//  ----------------------------------------------------------------------------
void VulkanRenderSystem::draw_models(
    std::vector<render::ModelBatch>& batches
) {
    m_render_task_mgr->draw_models(*m_model_renderer, batches);
}

//  ----------------------------------------------------------------------------
void VulkanRenderSystem::draw_sprites(
    std::vector<render::SpriteBatch>& batches
) {
    m_render_task_mgr->draw_sprites(*m_sprite_renderer, batches);
}

//  ----------------------------------------------------------------------------
void VulkanRenderSystem::end_frame() {
    //  Wait for worker threads to complete
    while (!check_render_tasks_complete()) {
        std::this_thread::sleep_for(std::chrono::microseconds(100));
    }

    //  Check that frame is OK to continue with
    if (m_frame_status != FrameStatus::Ready) {
        m_render_task_mgr->end_frame();
        return;
    }

    Frame& frame = m_frames.at(m_current_frame);

    std::vector<VkCommandBuffer> secondary_command_buffers;
    m_render_task_mgr->get_command_buffers(secondary_command_buffers);

    //  Record primary command buffers
    record_primary_command_buffer(
        m_render_pass,
        m_swapchain.extent,
        m_swapchain.framebuffers.at(m_image_index),
        secondary_command_buffers,
        frame.command.buffer
    );

    VkSubmitInfo submit_info{};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &frame.command.buffer;

    //  Wait for swapchain image before executing command buffers
    submit_info.waitSemaphoreCount = 1;
    submit_info.pWaitSemaphores = &frame.sync.image_acquired;
    VkPipelineStageFlags wait_stages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submit_info.pWaitDstStageMask = wait_stages;

    //  Wait for command buffers to finish before presenting
    submit_info.signalSemaphoreCount = 1;
    submit_info.pSignalSemaphores = &frame.sync.present_ready;

    vkResetFences(m_device, 1, &frame.sync.frame_complete);

    //  Submit draw commands
    STOPWATCH.start("queue_submit");
    if (m_graphics_queue.submit(1, submit_info, frame.sync.frame_complete) != VK_SUCCESS) {
        throw std::runtime_error("Failed to submit draw command buffer.");
    }
    STOPWATCH.stop("queue_submit");

    //  Presentation
    VkPresentInfoKHR present_info{};
    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores = &frame.sync.present_ready;
    present_info.swapchainCount = 1;
    VkSwapchainKHR swapchains[] = { m_swapchain.swapchain };
    present_info.pSwapchains = swapchains;
    present_info.pImageIndices = &m_image_index;

    //  Submit request to present image to swap chain
    begin_debug_marker(m_present_queue, "Present Frame", DEBUG_MARKER_COLOR_GREEN);
    STOPWATCH.start("queue_present");
    VkResult result = vkQueuePresentKHR(m_present_queue, &present_info);
    STOPWATCH.stop("queue_present");
    end_debug_marker(m_present_queue);

    //  Recreate swapchain if needed
    if (result == VK_ERROR_OUT_OF_DATE_KHR ||
        result == VK_SUBOPTIMAL_KHR ||
        m_framebuffer_resized
    ) {
        m_framebuffer_resized = false;
        recreate_swapchain();
    } else if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to present swap chain image.");
    }

    //  Advance frame counter
    m_current_frame = (m_current_frame + 1) % m_frame_count;

    m_render_task_mgr->end_frame();
}

//  ----------------------------------------------------------------------------
float VulkanRenderSystem::get_aspect_ratio() const {
    return m_swapchain.extent.width / (float)m_swapchain.extent.height;
}

//  ----------------------------------------------------------------------------
glm::vec2 VulkanRenderSystem::get_size() const {
    return { m_swapchain.extent.width, m_swapchain.extent.height };
}

//  ----------------------------------------------------------------------------
bool VulkanRenderSystem::initialize(GLFWwindow* glfw_window) {
    log_info("Initializing Vulkan renderer...");

    m_glfw_window = glfw_window;

    //  Check if Vulkan is supported
    if (!glfwVulkanSupported()) {
        log_error("Vulkan is not supported.");
        return false;
    }

    //  Initialize volk (loads Vulkan extensions loader)
    log_debug("Initializing volk...");
    if (volkInitialize() != VK_SUCCESS) {
        log_error("Could not initialize volk.");
        return false;
    }

    const std::vector<const char*> validation_layers = {
        #ifdef DEBUG
        "VK_LAYER_KHRONOS_validation"
        #endif
    };

    //  Create Vulkan instance
    if (!create_instance(m_instance, validation_layers, m_debug_messenger)) {
        return false;
    }

    //  Load all Vulkan entrypoints
    volkLoadInstance(m_instance);

    //  Create surface
    if (glfwCreateWindowSurface(m_instance, glfw_window, nullptr, &m_surface) != VK_SUCCESS) {
        log_error("Failed to create Vulkan window surface.");
        return false;
    }

    const std::vector<const char*> device_extensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    };

    //  Initialize physical device
    if (!init_device(
        m_instance,
        m_physical_device,
        m_surface,
        device_extensions
    )) {
        return false;
    }

    VkQueue graphics_queue = VK_NULL_HANDLE;

    //  Create logical device
    if (!create_logical_device(
        m_physical_device,
        m_surface,
        validation_layers,
        device_extensions,
        m_device,
        graphics_queue,
        m_present_queue
    )) {
        return false;
    }

    //  Add debug callback
    if (check_debug_utils_support()) {
        //  Call after device is created because debug util function pointers
        //  are loaded by create_logical_device
        create_debug_messenger(m_instance, m_debug_messenger);
    }

    //  Optimize device calls
    volkLoadDevice(m_device);

    m_graphics_queue.initialize(m_physical_device, m_device, graphics_queue);
    m_frame_uniform.create(m_physical_device, m_device);
    m_object_uniform.create(m_physical_device, m_device);

    create_descriptor_set_layouts(m_device, MAX_TEXTURES, m_descriptor_set_layouts);

    m_model_mgr = std::make_unique<ModelManager>();
    m_texture_mgr = std::make_unique<TextureManager>(m_physical_device, m_device);

    m_billboard_renderer = std::make_unique<BillboardRenderer>(*m_model_mgr);
    m_model_renderer = std::make_unique<ModelRenderer>(*m_model_mgr);
    m_sprite_renderer = std::make_unique<SpriteRenderer>(*m_model_mgr);

    create_swapchain_objects();

    create_swapchain_dependents();

    create_frame_resources();

    m_model_mgr->initialize(
        m_physical_device,
        m_device,
        m_graphics_queue.get_queue(),
        m_resource_command_pool
    );

    m_render_task_mgr = std::make_unique<RenderTaskManager>(
        m_physical_device,
        m_device,
        m_descriptor_set_layouts,
        m_frame_uniform,
        m_object_uniform,
        *m_model_mgr,
        *m_texture_mgr
    );

    return true;
}

//  ----------------------------------------------------------------------------
void VulkanRenderSystem::load_model(const AssetId id, const std::string& path) {
    Mesh mesh;
    load_mesh(mesh, path);

    auto model = std::make_unique<VulkanModel>(id);
    model->load(
        m_physical_device,
        m_device,
        m_graphics_queue.get_queue(),
        m_resource_command_pool,
        mesh
    );

    m_model_mgr->add_model(std::move(model));
}

//  ----------------------------------------------------------------------------
void VulkanRenderSystem::load_texture(const AssetId id, const std::string& path) {
    m_texture_mgr->load_texture(id, path, m_graphics_queue, m_resource_command_pool);
}

//  ----------------------------------------------------------------------------
void VulkanRenderSystem::recreate_swapchain() {
    //  Get window size
    int width;
    int height;
    glfwGetWindowSize(m_glfw_window, &width, &height);

    //  Handle window minimize
    while (width == 0 || height == 0) {
        glfwGetWindowSize(m_glfw_window, &width, &height);
        glfwWaitEvents();
    }

    vkDeviceWaitIdle(m_device);

    m_render_task_mgr->cancel_threads();

    destroy_frame_resources();

    //  Destroy all objects related to old swapchain
    destroy_swapchain();

    create_swapchain_objects();

    create_swapchain_dependents();

    create_frame_resources();

    m_render_task_mgr->start_threads();
}

//  ----------------------------------------------------------------------------
void VulkanRenderSystem::resize() {
    m_framebuffer_resized = true;
}

//  ----------------------------------------------------------------------------
void VulkanRenderSystem::shutdown() {
    log_debug("Shutting down Vulkan renderer...");

    //  Wait for operations to finish
    vkDeviceWaitIdle(m_device);

    m_render_task_mgr->cancel_threads();

    destroy_frame_resources();

    destroy_swapchain();

    vkDestroyDescriptorSetLayout(m_device, m_descriptor_set_layouts.frame, nullptr);
    // vkDestroyDescriptorSetLayout(m_device, m_descriptor_set_layouts.object, nullptr);
    vkDestroyDescriptorSetLayout(m_device, m_descriptor_set_layouts.texture_sampler, nullptr);

    m_frame_uniform.destroy();
    m_object_uniform.destroy();

    //  Unload models
    m_model_mgr->unload(m_device);

    m_texture_mgr->destroy_textures();

    vkDestroyDevice(m_device, nullptr);
    vkDestroySurfaceKHR(m_instance, m_surface, nullptr);

    if (m_debug_messenger != VK_NULL_HANDLE) {
        vkDestroyDebugUtilsMessengerEXT(m_instance, m_debug_messenger, nullptr);
    }

    vkDestroyInstance(m_instance, nullptr);
}

//  ----------------------------------------------------------------------------
void VulkanRenderSystem::update_frame_uniforms(
    const glm::mat4& view,
    const glm::mat4& proj,
    const glm::mat4& ortho_view,
    const glm::mat4& ortho_proj
) {
    m_render_task_mgr->update_frame_uniforms(view, proj, ortho_view, ortho_proj);
}
}
