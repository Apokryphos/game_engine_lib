#pragma once

#include "common/job_queue.hpp"
#include "render/texture_load_args.hpp"
#include "render_vk/vulkan.hpp"
#include <future>
#include <memory>
#include <mutex>
#include <optional>
#include <queue>
#include <thread>
#include <vector>

namespace render_vk
{
class ModelManager;
class Texture;
class TextureManager;
class VulkanQueue;

class AssetTaskManager
{
public:
    struct Job;

    template <typename T>
    using JobPromise = std::optional<std::promise<T>>;

private:
    enum class TaskId
    {
        None,
        LoadModel,
        LoadTexture,
    };

    static const char* task_id_to_string(TaskId task_id);


    //  Objects for worker threads
    struct ThreadState
    {
        VkCommandPool command_pool;
    };

    //  Number of worker threads.
    uint8_t m_thread_count {2};

    VkPhysicalDevice m_physical_device {VK_NULL_HANDLE};
    VkDevice m_device                  {VK_NULL_HANDLE};

    //  Worker threads
    std::vector<std::thread> m_threads;

    //  Job queue
    common::JobQueue<std::unique_ptr<Job>> m_jobs;

    VulkanQueue& m_queue;
    ModelManager& m_model_mgr;
    TextureManager& m_texture_mgr;

    //  Adds a new job for a worker thread to process.
    void add_job(std::unique_ptr<Job> job);
    void thread_main(uint8_t thread_id);

public:
    AssetTaskManager(
        VkPhysicalDevice physical_device,
        VkDevice device,
        VulkanQueue& queue,
        ModelManager& model_mgr,
        TextureManager& texture_mgr
    );
    ~AssetTaskManager();
    AssetTaskManager(const AssetTaskManager&) = delete;
    AssetTaskManager& operator=(const AssetTaskManager&) = delete;
    void cancel_threads();
    //  Enqueues a load model job for worker threads to complete
    void load_model(uint32_t id, const std::string& path);
    //  Enqueues a load texture job for worker threads to complete
    void load_texture(
        uint32_t id,
        const std::string& path,
        const render::TextureLoadArgs& args,
        JobPromise<bool> promise = {}
    );
    void start_threads();
};
}
