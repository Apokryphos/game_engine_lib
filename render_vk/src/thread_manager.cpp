#include "common/log.hpp"
#include "render_vk/command_pool.hpp"
#include "render_vk/mesh.hpp"
#include "render_vk/model_manager.hpp"
#include "render_vk/thread_manager.hpp"
#include "render_vk/vulkan_model.hpp"
#include "render_vk/vulkan_queue.hpp"
#include <algorithm>
#include <cassert>
#include <chrono>

using namespace common;

namespace render_vk
{
const unsigned MAX_THREADS = 4;
const int64_t WORK_THREAD_SLEEP_MS = 200;

//  ----------------------------------------------------------------------------
std::unique_ptr<VulkanModel> work_load_model(
    const ThreadManager::WorkState& state,
    const ThreadManager::LoadModelData& data
) {
    Mesh mesh;
    load_mesh(mesh, data.path);

    auto model = std::make_unique<VulkanModel>(data.id, data.path);
    model->load(
        state.physical_device,
        state.device,
        state.graphics_queue->get_queue(),
        state.command_pool,
        mesh
    );

    return model;
}

//  ----------------------------------------------------------------------------
Texture work_load_texture(
    const ThreadManager::WorkState& state,
    const ThreadManager::LoadTextureData& data
) {
    Texture texture{};

    create_texture(
        state.physical_device,
        state.device,
        *state.graphics_queue,
        state.command_pool,
        data.path,
        texture
    );

    return texture;
}

//  ----------------------------------------------------------------------------
ThreadManager::ThreadManager()
: m_threads_quit(false) {
}

//  ----------------------------------------------------------------------------
ThreadManager::~ThreadManager() {
    shutdown();
}

//  ----------------------------------------------------------------------------
bool ThreadManager::get_job(Job& job) {
    std::lock_guard<std::mutex> lock(m_jobs_mutex);
    if (m_jobs.empty()) {
        return false;
    }

    job = m_jobs.front();
    m_jobs.pop();
    return true;
}

//  ----------------------------------------------------------------------------
void ThreadManager::initialize(
    VkPhysicalDevice physical_device,
    VkDevice device,
    VulkanQueue& graphics_queue,
    ModelManager& model_mgr
) {
    assert(m_threads.empty());

    m_model_mgr = &model_mgr;

    const auto hw_threads = std::clamp<unsigned>(
        std::thread::hardware_concurrency(),
        1,
        MAX_THREADS
    );
    log_debug("%d concurrent threads supported.", hw_threads);

    //  Create work threads
    m_threads.reserve(hw_threads);
    for (int n = 0; n < hw_threads; ++n) {
        WorkState state{};
        state.id = n;
        state.physical_device = physical_device;
        state.device = device;
        state.graphics_queue = &graphics_queue;

        std::thread t(&ThreadManager::work, this, state);

        m_threads.push_back(std::move(t));
    }
}

//  ----------------------------------------------------------------------------
void ThreadManager::load_model(AssetId id, const std::string& path) {
    LoadModelData data{};
    data.id = id;
    data.path = path;

    queue_job(JobType::LoadModel, data);
}

//  ----------------------------------------------------------------------------
void ThreadManager::load_texture(AssetId id, const std::string& path) {
    LoadTextureData data{};
    data.id = id;
    data.path = path;

    queue_job(JobType::LoadTexture, data);
}

//  ----------------------------------------------------------------------------
void ThreadManager::shutdown() {
    //  Terminate threads
    m_threads_quit = true;
    for (auto& thread : m_threads) {
        thread.join();
    }
    m_threads.clear();
}

//  ----------------------------------------------------------------------------
void ThreadManager::update() {
}

//  ----------------------------------------------------------------------------
void ThreadManager::work(WorkState state) {
    //  Initialize thread-owned state objects
    create_command_pool(
        state.device,
        state.physical_device,
        state.command_pool
    );

    state.command_buffer = VK_NULL_HANDLE;

    //  Main loop
    while (!m_threads_quit) {
        //  Get next job
        Job job{};
        if (!get_job(job)) {
            std::this_thread::sleep_for(std::chrono::milliseconds(WORK_THREAD_SLEEP_MS));
            continue;
        }

        //  Process job
        switch (job.type) {
            default:
                log_error("Unsupported job.");
                break;

            case JobType::LoadModel: {
                LoadModelData* data = static_cast<LoadModelData*>(job.data);
                auto model = work_load_model(state, *data);
                m_model_mgr->add_model(data->id, std::move(model));
                delete(data);
                break;
            }

            case JobType::LoadTexture: {
                LoadTextureData* data = static_cast<LoadTextureData*>(job.data);
                Texture texture = work_load_texture(state, *data);
                m_model_mgr->add_texture(data->id, texture);
                delete(data);
                break;
            }
        }
    }

    //  Destroy thread-owned objects
    vkDestroyCommandPool(state.device, state.command_pool, nullptr);
}
}
