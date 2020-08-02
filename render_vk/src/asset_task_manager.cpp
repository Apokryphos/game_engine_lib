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

namespace render_vk
{
//  ----------------------------------------------------------------------------
const char* AssetTaskManager::task_id_to_string(TaskId task_id) {
    switch (task_id) {
        default:
            return "?";
        case TaskId::LoadModel:
            return "load_model";
        case TaskId::LoadTexture:
            return "load_texture";
    }
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
    m_jobs.push(job);
}

//  ----------------------------------------------------------------------------
void AssetTaskManager::cancel_threads() {
    m_jobs.cancel();
    for (std::thread& thread : m_threads) {
        thread.join();
    }
    m_threads.clear();
    m_jobs.resume();
}

//  ----------------------------------------------------------------------------
void AssetTaskManager::load_model(uint32_t id, const std::string& path) {
    Job job{};
    job.task_id = TaskId::LoadModel;
    job.asset_id = id;
    job.path = path;
    add_job(job);
}

//  ----------------------------------------------------------------------------
void AssetTaskManager::load_texture(uint32_t id, const std::string& path) {
    Job job{};
    job.task_id = TaskId::LoadTexture;
    job.asset_id = id;
    job.path = path;
    add_job(job);
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
    Stopwatch stopwatch;

    log_debug("Asset thread %d started (%p).", thread_id, std::this_thread::get_id());

    const std::string thread_name = "asset_worker" + std::to_string(thread_id);

    //  Initialize thread objects
    ThreadState state{};

    create_transient_command_pool(
        m_device,
        m_physical_device,
        state.command_pool,
        (thread_name + "_command_pool").c_str()
    );

    //  Main loop
    while (true) {
        Job job{};
        //  Sleep until job is available
        if (!m_jobs.wait_and_pop(job)) {
            break;
        }

        log_debug(
            "%s: execute %s",
            thread_name.c_str(),
            task_id_to_string(job.task_id)
        );

        //  Process job
        switch (job.task_id) {
            case TaskId::LoadModel: {
                stopwatch.start(thread_name+"_load_model");
                m_model_mgr.load_model(
                    job.asset_id,
                    job.path,
                    m_physical_device,
                    m_device,
                    m_queue,
                    state.command_pool
                );
                stopwatch.stop(thread_name+"_load_model");
                break;
            }

            case TaskId::LoadTexture: {
                stopwatch.start(thread_name+"_load_texture");
                m_texture_mgr.load_texture(
                    job.asset_id,
                    job.path,
                    m_queue,
                    state.command_pool
                );
                stopwatch.stop(thread_name+"_load_texture");
                break;
            }

            default:
                throw std::runtime_error("Asset worker thread task not implemented.");
        }
    }

    //  Release thread state objects
    vkDestroyCommandPool(m_device, state.command_pool, nullptr);

    log_debug("Asset worker thread %d exited.", thread_id);
}
}
