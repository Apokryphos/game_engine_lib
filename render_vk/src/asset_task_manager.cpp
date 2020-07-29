#include "common/log.hpp"
#include "common/stopwatch.hpp"
#include "render_vk/asset_task_manager.hpp"
#include "render_vk/command_buffer.hpp"
#include "render_vk/command_pool.hpp"
#include "render_vk/debug_utils.hpp"
#include "render_vk/model_manager.hpp"
#include "render_vk/texture.hpp"
#include "render_vk/texture_manager.hpp"
#include "render_vk/vulkan_queue.hpp"

using namespace common;
using namespace render;

namespace render_vk
{
static Stopwatch STOPWATCH;

//  ----------------------------------------------------------------------------
static void create_command_objects(
    VkDevice device,
    VkPhysicalDevice physical_device,
    const std::string& name_prefix,
    FrameCommandObjects& frame_command
) {
    create_transient_command_pool(
        device,
        physical_device,
        frame_command.pool,
        (name_prefix + "_command_pool").c_str()
    );

    create_primary_command_buffer(
        device,
        frame_command.pool,
        frame_command.buffer,
        (name_prefix + "_command_buffer").c_str()
    );
}

//  ----------------------------------------------------------------------------
AssetTaskManager::AssetTaskManager(
    VkPhysicalDevice physical_device,
    VkDevice device,
    VulkanQueue& queue,
    ModelManager& model_mgr,
    TextureManager& texture_mgr
)
: m_physical_device(physical_device),
  m_device(device),
  m_queue(queue),
  m_model_mgr(model_mgr),
  m_texture_mgr(texture_mgr) {
}

//  ----------------------------------------------------------------------------
AssetTaskManager::~AssetTaskManager() {
    cancel_threads();
}

//  ----------------------------------------------------------------------------
void AssetTaskManager::add_job(Job& job) {
    //  Check that a valid task ID was assigned
    if (job.task_id == TaskId::None) {
        throw std::runtime_error("Invalid job task ID.");
    }

    //  Enqueue job
    {
        std::lock_guard<std::mutex> lock(m_jobs_mutex);
        m_jobs.push(job);
    }
}

//  ----------------------------------------------------------------------------
void AssetTaskManager::cancel_threads() {
    m_cancel_threads = true;
    for (std::thread& thread : m_threads) {
        thread.join();
    }
    m_threads.clear();
    m_cancel_threads = false;
}

//  ----------------------------------------------------------------------------
bool AssetTaskManager::get_job(Job& job) {
    std::lock_guard<std::mutex> lock(m_jobs_mutex);

    if (m_jobs.empty()) {
        return false;
    }

    job = m_jobs.front();
    m_jobs.pop();

    return true;
}

//  ----------------------------------------------------------------------------
void AssetTaskManager::post_texture_results(
    TaskId task_id,
    uint32_t asset_id,
    Texture& texture
) {
    m_texture_mgr.add_texture(asset_id, texture);
}

//  ----------------------------------------------------------------------------
void AssetTaskManager::start_threads() {
    assert(m_thread_count > 0);
    for (auto n = 0; n < m_thread_count; ++n) {
        m_threads.emplace_back(&AssetTaskManager::thread_main, this, n);
    }
}

//  ----------------------------------------------------------------------------
void AssetTaskManager::thread_main(uint8_t thread_id) {
    log_debug("Asset worker thread %d started.", thread_id);

    const std::string thread_name = "asset_worker" + std::to_string(thread_id);

    //  Initialize thread objects
    ThreadState state{};

    create_command_objects(
        m_device,
        m_physical_device,
        thread_name,
        state.command
    );

    //  Main loop
    while (!m_cancel_threads) {
        Job job{};
        //  Sleep until job is available
        if (!get_job(job)) {
            std::this_thread::sleep_for(std::chrono::microseconds(250));
            continue;
        }

        vkResetCommandPool(
            m_device,
            state.command.pool,
            VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT
        );

        //  Process job
        switch (job.task_id) {
            case TaskId::LoadModel: {
                STOPWATCH.start(thread_name+"_load_model");
                throw std::runtime_error("Not implemented");
                STOPWATCH.stop(thread_name+"_load_model");
                break;
            }

            case TaskId::LoadTexture: {
                STOPWATCH.start(thread_name+"_load_texture");

                Texture texture{};
                create_texture(
                    m_physical_device,
                    m_device,
                    m_queue,
                    state.command.pool,
                    job.path,
                    texture
                );

                post_texture_results(job.task_id, job.asset_id, texture);

                STOPWATCH.stop(thread_name+"_draw_texture");
                break;
            }
        }

        //  Post completed work
    }

    //  Release thread state objects
    vkDestroyCommandPool(m_device, state.command.pool, nullptr);

    log_debug("Asset worker thread %d exited.", thread_id);
}
}
