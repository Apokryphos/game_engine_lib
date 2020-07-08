#include "common/log.hpp"
#include "platform/glfw_init.hpp"
#include "render_vk/command_buffer.hpp"
#include "render_vk/command_pool.hpp"
#include "render_vk/depth.hpp"
#include "render_vk/descriptor_pool.hpp"
#include "render_vk/descriptor_sets.hpp"
#include "render_vk/devices.hpp"
#include "render_vk/framebuffers.hpp"
#include "render_vk/frame_sync.hpp"
#include "render_vk/graphics_pipeline.hpp"
#include "render_vk/imgui/imgui_vk.hpp"
#include "render_vk/index_buffer.hpp"
#include "render_vk/queue_family.hpp"
#include "render_vk/mesh.hpp"
#include "render_vk/model_manager.hpp"
#include "render_vk/render_pass.hpp"
#include "render_vk/texture.hpp"
#include "render_vk/ubo_dynamic.hpp"
#include "render_vk/uniform.hpp"
#include "render_vk/vertex_buffer.hpp"
#include "render_vk/vulkan.hpp"
#include "render_vk/vulkan_model.hpp"
#include "render_vk/vulkan_renderer.hpp"
#include "render_vk/vulkan_swapchain.hpp"
#include <glm/mat4x4.hpp>
#include <cstdint>
#include <cstring>

using namespace common;
using namespace platform;
using namespace render;

namespace render_vk
{
const int MAX_FRAMES_IN_FLIGHT = 2;

//  ----------------------------------------------------------------------------
bool static check_validation_layers_support(
    const std::vector<const char*>& validation_layers
) {
    //  Get number of available validation layers
    uint32_t layer_count;
    vkEnumerateInstanceLayerProperties(&layer_count, nullptr);

    //  Get available validation layers
    std::vector<VkLayerProperties> available_layers(layer_count);
    vkEnumerateInstanceLayerProperties(&layer_count, available_layers.data());

    //  List available validation layers
    log_debug("%d Vulkan layers available.", layer_count);
    for (const auto& layer_properties : available_layers) {
        log_debug("\t%s", layer_properties.layerName);
    }

    //  Verify that all requested validation layers exist
    for (const char* layer_name : validation_layers) {
        bool layer_found = false;

        for (const auto& layer_properties : available_layers) {
            if (strcmp(layer_name, layer_properties.layerName) == 0) {
                layer_found = true;
                break;
            }
        }

        if (!layer_found) {
            return false;
        }
    }

    return true;
}

//  ----------------------------------------------------------------------------
static void create_sync_objects(
    VkDevice device,
    const VulkanSwapchain& swapchain,
    std::vector<VkSemaphore>& image_available_semaphores,
    std::vector<VkSemaphore>& render_finished_semaphores,
    std::vector<VkFence>& in_flight_fences,
    std::vector<VkFence>& images_in_flight
) {
    create_sync_objects(
        device,
        swapchain,
        MAX_FRAMES_IN_FLIGHT,
        image_available_semaphores,
        render_finished_semaphores,
        in_flight_fences,
        images_in_flight
    );
}

//  ----------------------------------------------------------------------------
static bool create_instance(
    VkInstance& instance,
    const std::vector<const char*>& validation_layers
) {
    VkApplicationInfo app_info{};
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pApplicationName = "render_vk";
    app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.pEngineName = "No Engine";
    app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.apiVersion = VK_API_VERSION_1_0;

    uint32_t count;
    const char** extensions = glfwGetRequiredInstanceExtensions(&count);
    if (extensions == NULL) {
        log_error("Vulkan is not supported.");
        return false;
    }

    VkInstanceCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    create_info.pApplicationInfo = &app_info;
    create_info.enabledExtensionCount = count;
    create_info.ppEnabledExtensionNames = extensions;
    create_info.enabledLayerCount = 0;

    //  Validation layers
    #ifdef DEBUG
    const bool enable_validation_layers = true;
    #else
    const bool enable_validation_layers = false;
    #endif

    if (enable_validation_layers) {
        log_debug("Initializing Vulkan validation layers.");
        if (!check_validation_layers_support(validation_layers)) {
            log_error("Vulkan validation layers unavailable.");
            return false;
        }

        create_info.enabledLayerCount = validation_layers.size();
        create_info.ppEnabledLayerNames = validation_layers.data();
    } else {
        create_info.enabledLayerCount = 0;
    }

    //  Vulkan instance
    if (vkCreateInstance(&create_info, nullptr, &instance) != VK_SUCCESS) {
        log_error("Failed to create Vulkan instance.");
        return false;
    }

    log_debug("Created Vulkan instance.");

    return true;
}

//  ----------------------------------------------------------------------------
static void query_extensions() {
    //  Get number of extensions
    uint32_t count = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &count, nullptr);

    //  Enumerate extensions
    std::vector<VkExtensionProperties> extensions(count);
    vkEnumerateInstanceExtensionProperties(nullptr, &count, extensions.data());

    log_debug("%d Vulkan extensions supported.", count);
    for (const auto& ext : extensions) {
        log_debug("\t%s", ext.extensionName);
    }
}

//  ----------------------------------------------------------------------------
VulkanRenderer::VulkanRenderer()
: m_framebuffer_resized(false),
  m_current_frame(0) {
}

//  ----------------------------------------------------------------------------
VulkanRenderer::~VulkanRenderer() {
}

//  ----------------------------------------------------------------------------
void VulkanRenderer::begin_frame() {
    m_draw_model_commands.clear();
}

//  ----------------------------------------------------------------------------
void VulkanRenderer::cleanup_swapchain() {
    //  Depth testing
    vkDestroyImageView(m_device, m_depth_image_view, nullptr);
    vkDestroyImage(m_device, m_depth_image, nullptr);
    vkFreeMemory(m_device, m_depth_image_memory, nullptr);

    //  Destroy framebuffers before respective images views and render pass
    for (auto framebuffer : m_swapchain.framebuffers) {
        vkDestroyFramebuffer(m_device, framebuffer, nullptr);
    }

    vkFreeCommandBuffers(
        m_device,
        m_command_pool,
        static_cast<uint32_t>(m_command_buffers.size()),
        m_command_buffers.data()
    );

    vkDestroyPipeline(m_device, m_graphics_pipeline, nullptr);
    vkDestroyPipelineLayout(m_device, m_pipeline_layout, nullptr);
    vkDestroyRenderPass(m_device, m_render_pass, nullptr);

    imgui_vulkan_cleanup_swapchain(m_device);

    //  Destroy image views
    for (size_t n = 0; n < m_swapchain.image_views.size(); ++n) {
        vkDestroyImageView(m_device, m_swapchain.image_views[n], nullptr);
    }

    destroy_dynamic_uniform_buffer(m_device, m_uniform_buffers, m_ubo_data_dynamic);

    // for (size_t n = 0; n < m_uniform_buffers.size(); n++) {
    //     vkDestroyBuffer(m_device, m_uniform_buffers[n], nullptr);
    //     vkFreeMemory(m_device, m_uniform_buffers_memory[n], nullptr);
    // }

    vkDestroyDescriptorPool(m_device, m_descriptor_pool, nullptr);

    //  Destroy swapchain
    vkDestroySwapchainKHR(m_device, m_swapchain.swapchain, nullptr);
}

//  ----------------------------------------------------------------------------
void VulkanRenderer::draw_frame(GLFWwindow* glfw_window) {
    vkWaitForFences(
        m_device,
        1,
        &m_in_flight_fences[m_current_frame],
        VK_TRUE,
        UINT64_MAX
    );

    uint32_t image_index;

    VkResult result = vkAcquireNextImageKHR(
        m_device,
        m_swapchain.swapchain,
        UINT64_MAX,
        m_image_available_semaphores[m_current_frame],
        VK_NULL_HANDLE,
        &image_index
    );

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        recreate_swapchain(glfw_window);
        return;
    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("Failed to acquire swapchain image.");
    }

    //  Check if previous frame is using this image
    if (m_images_in_flight[image_index] != VK_NULL_HANDLE) {
        vkWaitForFences(m_device, 1, &m_images_in_flight[image_index], VK_TRUE, UINT64_MAX);
    }

    // Mark the image as now being in use by this frame
    m_images_in_flight[image_index] = m_in_flight_fences[m_current_frame];

    update_uniform_buffer(image_index);

    //  Build command buffers
    record_command_buffer(
        m_render_pass,
        m_pipeline_layout,
        m_graphics_pipeline,
        m_draw_model_commands,
        m_descriptor_sets[image_index],
        m_swapchain.extent,
        m_swapchain.framebuffers[image_index],
        m_command_buffers[image_index],
        m_uniform_buffers.dynamic_align
    );

    VkSubmitInfo submit_info{};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    //  Semaphores to wait on before execution begins
    VkSemaphore wait_semaphores[] = { m_image_available_semaphores[m_current_frame] };
    VkPipelineStageFlags wait_stages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submit_info.waitSemaphoreCount = 1;
    submit_info.pWaitSemaphores = wait_semaphores;
    submit_info.pWaitDstStageMask = wait_stages;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &m_command_buffers[image_index];

    //  Semaphores to signal once command buffers have finished execution
    VkSemaphore signal_semaphores[] = { m_render_finished_semaphores[m_current_frame] };
    submit_info.signalSemaphoreCount = 1;
    submit_info.pSignalSemaphores = signal_semaphores;

    vkResetFences(m_device, 1, &m_in_flight_fences[m_current_frame]);

    if (vkQueueSubmit(
        m_graphics_queue,
        1,
        &submit_info,
        m_in_flight_fences[m_current_frame]
    ) != VK_SUCCESS) {
        throw std::runtime_error("Failed to submit draw command buffer.");
    }

    //  Presentation
    VkPresentInfoKHR present_info{};
    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores = signal_semaphores;

    //  Swapchains
    VkSwapchainKHR swapchains[] = { m_swapchain.swapchain };
    present_info.swapchainCount = 1;
    present_info.pSwapchains = swapchains;
    present_info.pImageIndices = &image_index;
    present_info.pResults = nullptr; // Optional

    //  Submit request to present image to swap chain
    result = vkQueuePresentKHR(m_present_queue, &present_info);

    if (result == VK_ERROR_OUT_OF_DATE_KHR ||
        result == VK_SUBOPTIMAL_KHR ||
        m_framebuffer_resized
    ) {
        m_framebuffer_resized = false;
        recreate_swapchain(glfw_window);
    } else if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to present swap chain image.");
    }

    m_current_frame = (m_current_frame + 1) % MAX_FRAMES_IN_FLIGHT;
}

//  ----------------------------------------------------------------------------
void VulkanRenderer::draw_model(
    const AssetId id,
    const glm::mat4x4& model,
    const glm::mat4x4& view,
    const glm::mat4x4& proj
) {
    VulkanModel* vk_model = m_model_mgr->get_model(id);

    VkBuffer index_buffer = vk_model->get_index_buffer();

    DrawModelCommand cmd{};
    cmd.index_count = vk_model->get_index_count();
    cmd.model = model;
    cmd.view = view;
    cmd.proj = proj;

    //  GLM (OpenGL) uses inverted Y clip coordinate
    cmd.proj[1][1] *= -1;

    cmd.vertex_buffer = vk_model->get_vertex_buffer();
    cmd.index_buffer = vk_model->get_index_buffer();

    m_draw_model_commands.push_back(cmd);
}

//  ----------------------------------------------------------------------------
float VulkanRenderer::get_aspect_ratio() const {
    return m_swapchain.extent.width / (float)m_swapchain.extent.height;
}

//  ----------------------------------------------------------------------------
bool VulkanRenderer::initialize(GLFWwindow* glfw_window) {
    log_info("Initializing Vulkan renderer...");

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

    query_extensions();

    const std::vector<const char*> validation_layers = {
        "VK_LAYER_KHRONOS_validation"
    };

    //  Create Vulkan instance
    if (!create_instance(m_instance, validation_layers)) {
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
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    //  Get window size
    int width;
    int height;
    glfwGetWindowSize(glfw_window, &width, &height);

    //  Initialize physical device
    if (!init_device(
        m_instance,
        m_physical_device,
        m_surface,
        device_extensions
    )) {
        return false;
    }

    //  Create logical device
    if (!create_logical_device(
        m_physical_device,
        m_device,
        m_graphics_queue,
        m_present_queue,
        m_surface,
        validation_layers,
        device_extensions
    )) {
        return false;
    }

    //  Optimize device calls
    volkLoadDevice(m_device);

    //  Create swap chain
    if (!create_swapchain(
        m_device,
        m_physical_device,
        m_surface,
        width,
        height,
        m_swapchain
    )) {
        return false;
    }

    if (!create_render_pass(m_device, m_physical_device, m_swapchain, m_render_pass)) {
        return false;
    }

    create_descriptor_set_layout(
        m_device,
        m_descriptor_set_layout
    );

    if (!create_graphics_pipeline(
        m_device,
        m_swapchain,
        m_render_pass,
        m_descriptor_set_layout,
        m_pipeline_layout,
        m_graphics_pipeline
    )) {
        return false;
    }

    if (!create_command_pool(m_device, m_physical_device, m_surface, m_command_pool)) {
        return false;
    }

    create_depth_resources(
        m_physical_device,
        m_device,
        m_graphics_queue,
        m_command_pool,
        m_swapchain,
        m_depth_image,
        m_depth_image_view,
        m_depth_image_memory
    );

    if (!create_framebuffers(m_device, m_render_pass, m_depth_image_view, m_swapchain)) {
        return false;
    }

    create_texture_image(
        m_physical_device,
        m_device,
        m_graphics_queue,
        m_command_pool,
        m_texture_image,
        m_texture_image_memory
    );

    create_texture_image_view(m_device, m_texture_image, m_texture_image_view);

    create_texture_sampler(m_device, m_texture_sampler);

    create_descriptor_pool(
        m_device,
        m_swapchain,
        m_descriptor_pool
    );

    prepare_uniform_buffers(
        m_physical_device,
        m_device,
        m_uniform_buffers,
        m_ubo_data_dynamic
    );

    create_descriptor_sets(
        m_device,
        m_swapchain,
        m_descriptor_set_layout,
        m_descriptor_pool,
        m_texture_image_view,
        m_texture_sampler,
        m_uniform_buffers.dynamic,
        m_descriptor_sets
    );

    if (!create_command_buffers(
        m_device,
        m_command_pool,
        m_swapchain,
        m_command_buffers
    )) {
        return false;
    }

    create_sync_objects(
        m_device,
        m_swapchain,
        m_image_available_semaphores,
        m_render_finished_semaphores,
        m_in_flight_fences,
        m_images_in_flight
    );

    m_model_mgr = std::make_unique<ModelManager>(
        m_physical_device,
        m_device,
        m_graphics_queue,
        m_command_pool
    );

    imgui_vulkan_init(
        m_instance,
        m_physical_device,
        m_device,
        m_graphics_queue,
        m_swapchain,
        m_render_pass,
        m_command_pool
    );

    return true;
}

//  ----------------------------------------------------------------------------
void VulkanRenderer::load_model(AssetId id, const std::string& path) {
    m_model_mgr->load_model(id, path);
}

//  ----------------------------------------------------------------------------
void VulkanRenderer::recreate_swapchain(GLFWwindow* glfw_window) {
    //  Get window size
    int width;
    int height;
    glfwGetWindowSize(glfw_window, &width, &height);

    //  Handle window minimize
    while (width == 0 || height == 0) {
        glfwGetWindowSize(glfw_window, &width, &height);
        glfwWaitEvents();
    }

    vkDeviceWaitIdle(m_device);
    cleanup_swapchain();

    //  Create swap chain
    if (!create_swapchain(
        m_device,
        m_physical_device,
        m_surface,
        width,
        height,
        m_swapchain
    )) {
        throw std::runtime_error("Failed to recreate swapchain.");
    }

    if (!create_render_pass(m_device, m_physical_device, m_swapchain, m_render_pass)) {
        throw std::runtime_error("Failed to recreate render pass.");
    }

    if (!create_graphics_pipeline(
        m_device,
        m_swapchain,
        m_render_pass,
        m_descriptor_set_layout,
        m_pipeline_layout,
        m_graphics_pipeline
    )) {
        throw std::runtime_error("Failed to recreate graphics pipeline.");
    }

    create_depth_resources(
        m_physical_device,
        m_device,
        m_graphics_queue,
        m_command_pool,
        m_swapchain,
        m_depth_image,
        m_depth_image_view,
        m_depth_image_memory
    );

    if (!create_framebuffers(m_device, m_render_pass, m_depth_image_view, m_swapchain)) {
        throw std::runtime_error("Failed to recreate framebuffers.");
    }

    prepare_uniform_buffers(
        m_physical_device,
        m_device,
        m_uniform_buffers,
        m_ubo_data_dynamic
    );

    create_descriptor_pool(
        m_device,
        m_swapchain,
        m_descriptor_pool
    );

    create_descriptor_sets(
        m_device,
        m_swapchain,
        m_descriptor_set_layout,
        m_descriptor_pool,
        m_texture_image_view,
        m_texture_sampler,
        m_uniform_buffers.dynamic,
        m_descriptor_sets
    );

    if (!create_command_buffers(
        m_device,
        m_command_pool,
        m_swapchain,
        m_command_buffers
    )) {
        throw std::runtime_error("Failed to recreate command buffers.");
    }

    imgui_vulkan_recreate_swapchain(
        m_instance,
        m_physical_device,
        m_device,
        m_graphics_queue,
        m_swapchain,
        m_render_pass,
        m_command_pool
    );
}

//  ----------------------------------------------------------------------------
void VulkanRenderer::resize(GLFWwindow* glfw_window) {
    m_framebuffer_resized = true;
}

//  ----------------------------------------------------------------------------
void VulkanRenderer::shutdown() {
    log_debug("Shutting down Vulkan renderer...");

    //  Wait for operations to finish
    vkDeviceWaitIdle(m_device);

    cleanup_swapchain();

    //  Destroy texture sampler
    vkDestroySampler(m_device, m_texture_sampler, nullptr);

    //  Destroy texture image view
    vkDestroyImageView(m_device, m_texture_image_view, nullptr);

    //  Destroy texture
    vkDestroyImage(m_device, m_texture_image, nullptr);
    vkFreeMemory(m_device, m_texture_image_memory, nullptr);

    vkDestroyDescriptorSetLayout(m_device, m_descriptor_set_layout, nullptr);

    //  Unload models
    m_model_mgr->unload();

    //  Sync objects
    for (size_t n = 0; n < MAX_FRAMES_IN_FLIGHT; ++n) {
        vkDestroySemaphore(m_device, m_image_available_semaphores[n], nullptr);
        vkDestroySemaphore(m_device, m_render_finished_semaphores[n], nullptr);
        vkDestroyFence(m_device, m_in_flight_fences[n], nullptr);
    }

    vkDestroyCommandPool(m_device, m_command_pool, nullptr);

    vkDestroyDevice(m_device, nullptr);

    vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
    vkDestroyInstance(m_instance, nullptr);
}

//  ----------------------------------------------------------------------------
void VulkanRenderer::update_uniform_buffer(uint32_t image_index) {
    //  Update all UBOs once per frame
    char* uniform_char = (char*)m_ubo_data_dynamic.data;
    for (size_t n = 0; n < m_draw_model_commands.size(); ++n)  {
        const DrawModelCommand& cmd = m_draw_model_commands[n];

        UboData data;
        data.model = cmd.model;
        data.proj = cmd.proj;
        data.view = cmd.view;

        memcpy(
            &uniform_char[n * m_uniform_buffers.dynamic_align],
            &data,
            sizeof(UboData)
        );
    }

    //  Copy UBO data to buffer
    copy_ubo_data_dynamic(m_ubo_data_dynamic, m_uniform_buffers);
}
}
