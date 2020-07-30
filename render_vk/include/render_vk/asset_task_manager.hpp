#pragma once

#include "common/job_queue.hpp"
#include "render_vk/vulkan.hpp"
#include <mutex>
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
    enum class TaskId
    {
        None,
        LoadModel,
        LoadTexture,
    };

    struct Job
    {
        TaskId task_id {TaskId::None};
        uint32_t asset_id;
        std::string path;
    };

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
    common::JobQueue<Job> m_jobs;

    VulkanQueue& m_queue;
    ModelManager& m_model_mgr;
    TextureManager& m_texture_mgr;

    //  Adds a new job for a worker thread to process.
    void add_job(Job& job);
    //  Called by worker threads when work is completed.
    void post_texture_results(
        TaskId task_id,
        Texture& texture
    );
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
    void load_model(uint32_t id, const std::string& path);
    void load_texture(uint32_t id, const std::string& path);
    void start_threads();
};
}
