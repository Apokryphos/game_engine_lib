#include "common/thread_manager.hpp"
#include "render_vk/command_buffer.hpp"
#include "render_vk/command_pool.hpp"
#include "render_vk/render_job_manager.hpp"
#include "render_vk/render_job_manager.hpp"
#include "render_vk/render_tasks/draw_models_task.hpp"
#include "render_vk/render_tasks/load_model_task.hpp"
#include "render_vk/render_tasks/load_texture_task.hpp"
#include "render_vk/render_tasks/render_task_ids.hpp"
#include "render_vk/vulkan.hpp"

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
void RenderJobManager::draw_models(
    const std::vector<uint32_t>& model_ids,
    const std::vector<glm::vec3>& positions
) {
    DrawModelsArgs args{};
    args.model_ids = model_ids;
    args.positions = positions;

    m_thread_mgr.add_job(RENDER_TASK_DRAW_MODELS, args);
}

//  ----------------------------------------------------------------------------
void RenderJobManager::initialize(
    VkPhysicalDevice physical_device,
    VkDevice device,
    VulkanQueue& graphics_queue,
    ModelManager& model_mgr,
    uint32_t swapchain_image_count
) {
    assert(swapchain_image_count > 0);

    //  Initialize thread state lambda
    auto init_state = [physical_device, device, &graphics_queue, &model_mgr, swapchain_image_count]() {
        RenderThreadState state{};
        state.physical_device = physical_device;
        state.device = device;
        state.graphics_queue = &graphics_queue;
        state.model_mgr = &model_mgr;

        //  Each thread has its own command pool
        create_command_pool(
            state.device,
            state.physical_device,
            state.command_pool
        );

        create_secondary_command_buffers(
            state.device,
            state.command_pool,
            swapchain_image_count,
            state.command_buffers
        );

        return state;
    };

    //  Cleanup thread state lambda
    auto cleanup_state = [](RenderThreadState& state) {
        vkDestroyCommandPool(state.device, state.command_pool, nullptr);
    };

    //  Start threads
    m_thread_mgr.start_threads(init_state, cleanup_state, 4);
}

//  ----------------------------------------------------------------------------
void RenderJobManager::initialize_tasks() {
    m_thread_mgr.add_task<LoadModelArgs>(RENDER_TASK_LOAD_MODEL, task_load_model);
    m_thread_mgr.add_task<LoadTextureArgs>(RENDER_TASK_LOAD_TEXTURE, task_load_texture);
    m_thread_mgr.add_task<DrawModelsArgs>(RENDER_TASK_DRAW_MODELS, task_draw_models);
}

//  ----------------------------------------------------------------------------
void RenderJobManager::load_model(common::AssetId id, const std::string& path) {
    LoadModelArgs args{};
    args.id = id;
    args.path = path;

    m_thread_mgr.add_job(RENDER_TASK_LOAD_MODEL, args);
}

//  ----------------------------------------------------------------------------
void RenderJobManager::load_texture(common::AssetId id, const std::string& path) {
    LoadTextureArgs args{};
    args.id = id;
    args.path = path;

    m_thread_mgr.add_job(RENDER_TASK_LOAD_TEXTURE, args);
}

//  ----------------------------------------------------------------------------
void RenderJobManager::shutdown() {
    log_debug("Shutting down render job manager...");
    m_thread_mgr.cancel();
}
}
