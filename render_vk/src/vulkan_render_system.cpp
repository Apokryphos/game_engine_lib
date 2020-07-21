#include "common/log.hpp"
#include "common/stopwatch.hpp"
#include "platform/glfw_init.hpp"
#include "render_vk/command_buffer.hpp"
#include "render_vk/command_pool.hpp"
#include "render_vk/debug_utils.hpp"
#include "render_vk/depth.hpp"
#include "render_vk/devices.hpp"
#include "render_vk/framebuffers.hpp"
#include "render_vk/graphics_pipeline.hpp"
#include "render_vk/imgui/imgui_vk.hpp"
#include "render_vk/instance.hpp"
#include "render_vk/mesh.hpp"
#include "render_vk/model_manager.hpp"
#include "render_vk/render_pass.hpp"
#include "render_vk/render_tasks/task_draw_models.hpp"
#include "render_vk/render_tasks/task_update_uniforms.hpp"
#include "render_vk/texture.hpp"
#include "render_vk/vulkan.hpp"
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
static const uint32_t MAX_TEXTURES = 4096;
static Stopwatch STOPWATCH;

//  ----------------------------------------------------------------------------
static void create_sync_objects(
    VkDevice device,
    const std::string& name_prefix,
    VulkanRenderSystem::FrameSyncObjects& sync
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
void create_descriptor_pool(
    VkDevice device,
    VkDescriptorPool& descriptor_pool
) {
    const uint32_t sampler_count = MAX_TEXTURES;
    const uint32_t max_sets = 2 + (sampler_count);

    std::array<VkDescriptorPoolSize, 2> pool_sizes{};
    pool_sizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    pool_sizes[0].descriptorCount = 1;
    // pool_sizes[1].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    // pool_sizes[1].descriptorCount = 1;
    pool_sizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    pool_sizes[1].descriptorCount = sampler_count;

    VkDescriptorPoolCreateInfo pool_info{};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.poolSizeCount = static_cast<uint32_t>(pool_sizes.size());
    pool_info.pPoolSizes = pool_sizes.data();
    pool_info.maxSets = max_sets;

    if (vkCreateDescriptorPool(device, &pool_info, nullptr, &descriptor_pool) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create descriptor pool.");
    }
}

//  ----------------------------------------------------------------------------
static void create_descriptor_set(
    const VkDevice device,
    const VkDescriptorSetLayout descriptor_set_layout,
    const VkDescriptorPool descriptor_pool,
    const std::string& debug_name,
    VkDescriptorSet& descriptor_set
) {
    VkDescriptorSetAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    alloc_info.descriptorPool = descriptor_pool;
    alloc_info.descriptorSetCount = 1;
    alloc_info.pSetLayouts = &descriptor_set_layout;

    if (vkAllocateDescriptorSets(device, &alloc_info, &descriptor_set) != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate descriptor set.");
    }

    set_debug_name(
        device,
        VK_OBJECT_TYPE_DESCRIPTOR_SET,
        descriptor_set,
        debug_name.c_str()
    );
}

//  ----------------------------------------------------------------------------
void update_frame_descriptor_sets(
    VkDevice device,
    VkBuffer frame_uniform_buffer,
    size_t frame_ubo_size,
    VkDescriptorSet& descriptor_set
) {
    VkDescriptorBufferInfo frame_buffer_info{};
    frame_buffer_info.buffer = frame_uniform_buffer;
    frame_buffer_info.offset = 0;
    frame_buffer_info.range = frame_ubo_size;

    std::array<VkWriteDescriptorSet, 1> descriptor_writes{};

    //  Per-frame dynamic uniform buffer
    descriptor_writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptor_writes[0].dstSet = descriptor_set;
    descriptor_writes[0].dstBinding = 0;
    descriptor_writes[0].dstArrayElement = 0;
    descriptor_writes[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptor_writes[0].descriptorCount = 1;
    descriptor_writes[0].pBufferInfo = &frame_buffer_info;
    descriptor_writes[0].pImageInfo = nullptr;
    descriptor_writes[0].pTexelBufferView = nullptr;

    vkUpdateDescriptorSets(
        device,
        static_cast<uint32_t>(descriptor_writes.size()),
        descriptor_writes.data(),
        0,
        nullptr
    );
}

//  ----------------------------------------------------------------------------
void update_texture_descriptor_sets(
    VkDevice device,
    const std::vector<Texture>& textures,
    VkDescriptorSet& descriptor_set
) {
    std::vector<VkDescriptorImageInfo> image_infos(textures.size());
    for (size_t n = 0; n < image_infos.size(); ++n) {
        image_infos[n].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        image_infos[n].imageView = textures[n].view;
        image_infos[n].sampler = textures[n].sampler;
    }

    std::array<VkWriteDescriptorSet, 1> descriptor_writes{};

    //  Combined texture sampler
    descriptor_writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptor_writes[0].dstSet = descriptor_set;
    descriptor_writes[0].dstBinding = 0;
    descriptor_writes[0].dstArrayElement = 0;
    descriptor_writes[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptor_writes[0].descriptorCount = static_cast<uint32_t>(image_infos.size());
    descriptor_writes[0].pBufferInfo = nullptr;
    descriptor_writes[0].pImageInfo = image_infos.data();
    descriptor_writes[0].pTexelBufferView = nullptr;

    vkUpdateDescriptorSets(
        device,
        static_cast<uint32_t>(descriptor_writes.size()),
        descriptor_writes.data(),
        0,
        nullptr
    );
}

//  ----------------------------------------------------------------------------
void update_object_descriptor_sets(
    VkDevice device,
    const std::vector<Texture>& textures,
    VkBuffer object_uniform_buffer,
    VkDescriptorSet& descriptor_set
) {
    VkDescriptorBufferInfo object_buffer_info{};
    object_buffer_info.buffer = object_uniform_buffer;
    object_buffer_info.offset = 0;
    object_buffer_info.range = VK_WHOLE_SIZE;

    std::vector<VkDescriptorImageInfo> image_infos(textures.size());
    for (size_t n = 0; n < image_infos.size(); ++n) {
        image_infos[n].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        image_infos[n].imageView = textures[n].view;
        image_infos[n].sampler = textures[n].sampler;
    }

    std::array<VkWriteDescriptorSet, 2> descriptor_writes{};

    //  Per-object dynamic uniform buffer
    descriptor_writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptor_writes[0].dstSet = descriptor_set;
    descriptor_writes[0].dstBinding = 0;
    descriptor_writes[0].dstArrayElement = 0;
    descriptor_writes[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    descriptor_writes[0].descriptorCount = 1;
    descriptor_writes[0].pBufferInfo = &object_buffer_info;
    descriptor_writes[0].pImageInfo = nullptr;
    descriptor_writes[0].pTexelBufferView = nullptr;

    //  Combined texture sampler
    descriptor_writes[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptor_writes[1].dstSet = descriptor_set;
    descriptor_writes[1].dstBinding = 1;
    descriptor_writes[1].dstArrayElement = 0;
    descriptor_writes[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptor_writes[1].descriptorCount = static_cast<uint32_t>(image_infos.size());
    descriptor_writes[1].pBufferInfo = nullptr;
    descriptor_writes[1].pImageInfo = image_infos.data();
    descriptor_writes[1].pTexelBufferView = nullptr;

    vkUpdateDescriptorSets(
        device,
        static_cast<uint32_t>(descriptor_writes.size()),
        descriptor_writes.data(),
        0,
        nullptr
    );
}

//  ----------------------------------------------------------------------------
static void create_primary_command_objects(
    VkDevice device,
    VkPhysicalDevice physical_device,
    VulkanQueue& graphics_queue,
    VulkanSwapchain& swapchain,
    DepthImage depth_image,
    VulkanRenderSystem::FrameCommandObjects& frame_command
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
static void create_secondary_command_objects(
    VkDevice device,
    VkPhysicalDevice physical_device,
    VulkanQueue& graphics_queue,
    VulkanSwapchain& swapchain,
    DepthImage depth_image,
    const std::string& name_prefix,
    VulkanRenderSystem::FrameCommandObjects& frame_command
) {
    create_command_pool(
        device,
        physical_device,
        frame_command.pool,
        (name_prefix + "_command_pool").c_str()
    );

    create_secondary_command_buffer(
        device,
        frame_command.pool,
        frame_command.buffer,
        (name_prefix + "_command_buffer").c_str()
    );
}

//  ----------------------------------------------------------------------------
static void create_descriptor_objects(
    VkDevice device,
    DescriptorSetLayouts descriptor_set_layouts,
    const std::vector<Texture>& textures,
    const UniformBuffer<FrameUbo>& frame_uniform,
    const DynamicUniformBuffer<ObjectUbo>& object_uniform,
    const std::string& name_prefix,
    VulkanRenderSystem::FrameDescriptorObjects& descriptor
) {
    create_descriptor_pool(device, descriptor.pool);

    create_descriptor_set(
        device,
        descriptor_set_layouts.frame,
        descriptor.pool,
        name_prefix + "_frame_descriptor_set",
        descriptor.frame_set
    );

    create_descriptor_set(
        device,
        descriptor_set_layouts.texture_sampler,
        descriptor.pool,
        name_prefix + "_texture_sampler_descriptor_set",
        descriptor.texture_set
    );

    // create_descriptor_set(
    //     device,
    //     descriptor_set_layouts.object,
    //     descriptor.pool,
    //     name_prefix + "_object_descriptor_set",
    //     descriptor.object_set
    // );

    update_frame_descriptor_sets(
        device,
        frame_uniform.get_buffer(),
        frame_uniform.get_ubo_size(),
        descriptor.frame_set
    );

    update_texture_descriptor_sets(
        device,
        textures,
        descriptor.texture_set
    );

    // update_object_descriptor_sets(
    //     device,
    //     textures,
    //     object_uniform.get_buffer(),
    //     descriptor.object_set
    // );
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

    //  Check that textures exist
    std::vector<Texture> textures;
    m_model_mgr->get_textures(textures);
    if (textures.empty()) {
        m_frame_status = FrameStatus::Discarded;
        return;
    }

    static bool first_call = true;
    if (first_call) {
        first_call = false;
        start_threads();
    }

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
        return;
    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("Failed to acquire swapchain image.");
    }

    m_frame_status = FrameStatus::Busy;

    STOPWATCH.stop("begin_frame");
}

//  ----------------------------------------------------------------------------
void VulkanRenderSystem::cancel_threads() {
    m_cancel_threads = true;
    for (std::thread& thread : m_threads) {
        thread.join();
    }
    m_threads.clear();
    m_cancel_threads = false;
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
            m_graphics_queue,
            m_swapchain,
            m_depth_image,
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

    create_graphics_pipeline(
        m_device,
        m_swapchain,
        m_render_pass,
        m_descriptor_set_layouts,
        m_pipeline_layout,
        m_graphics_pipeline
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

    vkDestroyPipeline(m_device, m_graphics_pipeline, nullptr);
    vkDestroyPipelineLayout(m_device, m_pipeline_layout, nullptr);

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
void VulkanRenderSystem::draw_models(
    std::vector<ModelBatch>& batches
) {
    //  Draw models
    Job job{};
    job.task_id = FrameTaskId::DrawModels;
    job.batches = batches;
    {
        std::lock_guard<std::mutex> lock(m_jobs_mutex);
        m_jobs.push(job);
    }
}

//  ----------------------------------------------------------------------------
void VulkanRenderSystem::end_frame() {
    //  Wait for worker threads to complete
    while (m_frame_status == FrameStatus::Busy) {
        std::this_thread::sleep_for(std::chrono::microseconds(100));
    }

    //  Check that frame is OK to continue with
    if (m_frame_status != FrameStatus::Ready) {
        return;
    }

    Frame& frame = m_frames.at(m_current_frame);

    //  Record primary command buffers
    record_primary_command_buffer(
        m_render_pass,
        m_pipeline_layout,
        m_graphics_pipeline,
        m_swapchain.extent,
        m_swapchain.framebuffers.at(m_image_index),
        m_tasks.get_command_buffers(FrameTaskId::DrawModels).at(0),
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

    m_tasks.clear();
}

//  ----------------------------------------------------------------------------
float VulkanRenderSystem::get_aspect_ratio() const {
    return m_swapchain.extent.width / (float)m_swapchain.extent.height;
}

//  ----------------------------------------------------------------------------
bool VulkanRenderSystem::get_job(Job& job) {
    std::lock_guard<std::mutex> lock(m_jobs_mutex);

    if (m_jobs.empty()) {
        return false;
    }

    job = m_jobs.front();
    m_jobs.pop();

    return true;
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

    create_swapchain_objects();

    create_swapchain_dependents();

    create_frame_resources();

    m_model_mgr = std::make_unique<ModelManager>();

    return true;
}

//  ----------------------------------------------------------------------------
void VulkanRenderSystem::load_model(const AssetId id, const std::string& path) {
    Mesh mesh;
    load_mesh(mesh, path);

    auto model = std::make_unique<VulkanModel>(id, path);
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
    Texture texture{};

    create_texture(
        m_physical_device,
        m_device,
        m_graphics_queue,
        m_resource_command_pool,
        path,
        texture
    );

    m_model_mgr->add_texture(id, texture);
}

//  ----------------------------------------------------------------------------
void VulkanRenderSystem::post_work(
    FrameTaskId task_id,
    VkCommandBuffer command_buffer
) {
    std::lock_guard<std::mutex> lock(m_tasks_mutex);

    m_tasks.add_results(task_id, command_buffer);

    if (m_tasks.get_count(FrameTaskId::DrawModels) &&
        m_tasks.get_count(FrameTaskId::UpdateFrameUniforms)
    ) {
        m_frame_status = FrameStatus::Ready;
    }
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

    cancel_threads();

    destroy_frame_resources();

    //  Destroy all objects related to old swapchain
    destroy_swapchain();

    create_swapchain_objects();

    create_swapchain_dependents();

    create_frame_resources();

    start_threads();
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

    cancel_threads();

    destroy_frame_resources();

    destroy_swapchain();

    vkDestroyDescriptorSetLayout(m_device, m_descriptor_set_layouts.frame, nullptr);
    // vkDestroyDescriptorSetLayout(m_device, m_descriptor_set_layouts.object, nullptr);
    vkDestroyDescriptorSetLayout(m_device, m_descriptor_set_layouts.texture_sampler, nullptr);

    m_frame_uniform.destroy();
    m_object_uniform.destroy();

    //  Unload models
    m_model_mgr->unload(m_device);

    vkDestroyDevice(m_device, nullptr);
    vkDestroySurfaceKHR(m_instance, m_surface, nullptr);

    if (m_debug_messenger != VK_NULL_HANDLE) {
        vkDestroyDebugUtilsMessengerEXT(m_instance, m_debug_messenger, nullptr);
    }

    vkDestroyInstance(m_instance, nullptr);
}

//  ----------------------------------------------------------------------------
void VulkanRenderSystem::start_threads() {
    assert(m_thread_count > 0);
    for (auto n = 0; n < m_thread_count; ++n) {
        m_threads.emplace_back(&VulkanRenderSystem::thread_main, this, n);
    }
}

//  ----------------------------------------------------------------------------
void VulkanRenderSystem::thread_main(uint8_t thread_id) {
    log_debug("Thread %d started.", thread_id);

    const std::string thread_name = "thread" + std::to_string(thread_id);

    std::vector<Texture> textures;
    m_model_mgr->get_textures(textures);

    //  Initialize frame objects
    uint32_t frame_index = 0;
    std::vector<ThreadFrame> frames(m_frame_count);
    for (ThreadFrame& frame : frames) {
        const std::string frame_name =
            thread_name + "_frame" + std::to_string(++frame_index);

        create_secondary_command_objects(
            m_device,
            m_physical_device,
            m_graphics_queue,
            m_swapchain,
            m_depth_image,
            frame_name,
            frame.command
        );

        create_descriptor_objects(
            m_device,
            m_descriptor_set_layouts,
            textures,
            m_frame_uniform,
            m_object_uniform,
            frame_name,
            frame.descriptor
        );
    }

    //  Main loop
    while (!m_cancel_threads) {
        Job job{};
        //  Sleep until job is available
        if (!get_job(job)) {
            std::this_thread::sleep_for(std::chrono::microseconds(250));
            continue;
        }

        //  Process job
        ThreadFrame& frame = frames.at(m_current_frame);

        vkResetCommandPool(
            m_device,
            frame.command.pool,
            VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT
        );

        switch (job.task_id) {
            case FrameTaskId::DrawModels:
                STOPWATCH.start(thread_name+"_draw_models");
                task_draw_models(
                    m_render_pass,
                    m_pipeline_layout,
                    m_graphics_pipeline,
                    *m_model_mgr,
                    frame.descriptor,
                    job.batches,
                    frame.command.buffer
                );
                STOPWATCH.stop(thread_name+"_draw_models");
                break;

            case FrameTaskId::UpdateFrameUniforms:
                STOPWATCH.start(thread_name+"_update_frame_uniforms");
                task_update_frame_uniforms(job.view, job.proj, m_frame_uniform);
                STOPWATCH.stop(thread_name+"_update_frame_uniforms");
                break;

            case FrameTaskId::UpdateObjectUniforms:
                STOPWATCH.start(thread_name+"_update_object_uniforms");
                task_update_object_uniforms(job.batches, m_object_uniform);
                STOPWATCH.stop(thread_name+"_update_object_uniforms");
                break;
        }

        //  Post completed work
        post_work(job.task_id, frame.command.buffer);
    }

    //  Release frame objects
    for (ThreadFrame& frame : frames) {
        //  Command pool
        vkDestroyCommandPool(m_device, frame.command.pool, nullptr);
        //  Descriptors
        vkDestroyDescriptorPool(m_device, frame.descriptor.pool, nullptr);
    }

    log_debug("Thread %d exited.", thread_id);
}

//  ----------------------------------------------------------------------------
void VulkanRenderSystem::update_frame_uniforms(
    const glm::mat4& view,
    const glm::mat4& proj
) {
    //  Update uniform data
    Job job{};
    job.task_id = FrameTaskId::UpdateFrameUniforms;
    job.view = view;
    job.proj = proj;

    {
        std::lock_guard<std::mutex> lock(m_jobs_mutex);
        m_jobs.push(job);
    }
}
}
