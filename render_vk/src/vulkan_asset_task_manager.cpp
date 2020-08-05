#include "common/log.hpp"
#include "common/stopwatch.hpp"
#include "render/texture_create_args.hpp"
#include "render_vk/command_buffer.hpp"
#include "render_vk/command_pool.hpp"
#include "render_vk/debug_utils.hpp"
#include "render_vk/model_manager.hpp"
#include "render_vk/spine.hpp"
#include "render_vk/texture.hpp"
#include "render_vk/texture_manager.hpp"
#include "render_vk/vulkan_queue.hpp"
#include "render_vk/vulkan_asset_task_manager.hpp"

using namespace assets;
using namespace common;
using namespace render;

namespace render_vk
{
struct VulkanAssetTaskManager::Job
{
    TaskId task_id {TaskId::None};
    uint32_t asset_id {0};
    std::string path;
};

struct SpineJob : VulkanAssetTaskManager::Job
{
    AssetManager* asset_mgr {nullptr};
    SpineAssetPromise promise;
    TextureCreateArgs args {};
};

struct TextureJob : VulkanAssetTaskManager::Job
{
    TextureAssetPromise promise;
    TextureCreateArgs args {};
};

//  ----------------------------------------------------------------------------
const char* VulkanAssetTaskManager::task_id_to_string(TaskId task_id) {
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
VulkanAssetTaskManager::VulkanAssetTaskManager(
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
VulkanAssetTaskManager::~VulkanAssetTaskManager() {
    cancel_threads();
}

//  ----------------------------------------------------------------------------
void VulkanAssetTaskManager::add_job(std::unique_ptr<Job> job) {
    //  Check that a valid task ID was assigned
    if (job->task_id == TaskId::None) {
        throw std::runtime_error("Invalid job task ID.");
    }

    //  Enqueue job
    m_jobs.push(std::move(job));
}

//  ----------------------------------------------------------------------------
void VulkanAssetTaskManager::cancel_threads() {
    m_jobs.cancel();
    for (std::thread& thread : m_threads) {
        thread.join();
    }
    m_threads.clear();
    m_jobs.resume();
}

//  ----------------------------------------------------------------------------
void VulkanAssetTaskManager::load_model(uint32_t id, const std::string& path) {
    auto job = std::make_unique<Job>();
    job->task_id = TaskId::LoadModel;
    job->asset_id = id;
    job->path = path;
    add_job(std::move(job));
}

//  ----------------------------------------------------------------------------
void VulkanAssetTaskManager::load_spine(
    AssetId id,
    SpineLoadArgs& load_args,
    const render::TextureCreateArgs& create_args
) {
    auto job = std::make_unique<SpineJob>();
    job->task_id = TaskId::LoadSpine;
    job->asset_id = id;
    job->asset_mgr = load_args.asset_mgr;
    job->path = load_args.path;
    job->args = create_args;
    job->promise = std::move(load_args.promise);
    add_job(std::move(job));
}

//  ----------------------------------------------------------------------------
void VulkanAssetTaskManager::load_texture(
    uint32_t id,
    TextureLoadArgs& load_args,
    const TextureCreateArgs& create_args
) {
    auto job = std::make_unique<TextureJob>();
    job->task_id = TaskId::LoadTexture;
    job->asset_id = id;
    job->path = load_args.path;
    job->args = create_args;
    job->promise = std::move(load_args.promise);
    add_job(std::move(job));
}

//  ----------------------------------------------------------------------------
void VulkanAssetTaskManager::shutdown() {
    cancel_threads();
}

//  ----------------------------------------------------------------------------
void VulkanAssetTaskManager::start_threads() {
    assert(m_thread_count > 0);
    for (auto n = 0; n < m_thread_count; ++n) {
        m_threads.emplace_back(&VulkanAssetTaskManager::thread_main, this, n);
    }
}

//  ----------------------------------------------------------------------------
void VulkanAssetTaskManager::thread_main(uint8_t thread_id) {
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
        std::unique_ptr<Job> job;
        //  Sleep until job is available
        if (!m_jobs.wait_and_pop(job)) {
            break;
        }

        log_debug(
            "%s: execute %s",
            thread_name.c_str(),
            task_id_to_string(job->task_id)
        );

        //  Process job
        switch (job->task_id) {
            case TaskId::LoadModel: {
                stopwatch.start(thread_name+"_load_model");
                m_model_mgr.load_model(
                    job->asset_id,
                    job->path,
                    m_physical_device,
                    m_device,
                    m_queue,
                    state.command_pool
                );
                stopwatch.stop(thread_name+"_load_model");
                break;
            }

            case TaskId::LoadSpine: {
                SpineJob* spine_job = static_cast<SpineJob*>(job.get());

                stopwatch.start(thread_name+"_load_spine");
                render_vk::load_spine(job->asset_id, job->path, *spine_job->asset_mgr);

                if (spine_job->promise.has_value()) {
                    SpineAsset spine_asset {};
                    spine_asset.id = job->asset_id;
                    spine_job->promise.value().set_value(spine_asset);
                }

                stopwatch.stop(thread_name+"_load_spine");
                break;
            }

            case TaskId::LoadTexture: {
                TextureJob* texture_job = static_cast<TextureJob*>(job.get());

                stopwatch.start(thread_name+"_load_texture");
                Texture texture = m_texture_mgr.load_texture(
                    job->asset_id,
                    job->path,
                    m_queue,
                    state.command_pool,
                    texture_job->args
                );

                if (texture_job->promise.has_value()) {
                    TextureAsset texture_asset {};
                    texture_asset.id = texture.id;
                    texture_asset.width = texture.width;
                    texture_asset.height = texture.height;
                    texture_job->promise.value().set_value(texture_asset);
                }

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
