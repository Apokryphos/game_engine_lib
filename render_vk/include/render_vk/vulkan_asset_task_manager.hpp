#pragma once

#include "assets/asset_task_manager.hpp"
#include "assets/texture_create_args.hpp"
#include "common/job_queue.hpp"
#include "render_vk/texture.hpp"
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
struct Mesh;
class ModelManager;
class Texture;
class TextureManager;
class VulkanQueue;
class VulkanSpineManager;

class VulkanAssetTaskManager : public assets::AssetTaskManager
{
public:
    struct Job;

private:
    enum class TaskId
    {
        None,
        LoadMesh,
        LoadModel,
        LoadSpine,
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
    VulkanSpineManager& m_spine_mgr;
    TextureManager& m_texture_mgr;

    //  Adds a new job for a worker thread to process.
    void add_job(std::unique_ptr<Job> job);
    void thread_load_model(ThreadState& state, Job* job);
    Texture thread_load_texture(
        const TextureId texture_id,
        const std::string& path,
        const assets::TextureCreateArgs& create_args,
        ThreadState& state
    );
    void thread_main(uint8_t thread_id);

public:
    VulkanAssetTaskManager(
        VkPhysicalDevice physical_device,
        VkDevice device,
        VulkanQueue& queue,
        ModelManager& model_mgr,
        VulkanSpineManager& spine_mgr,
        TextureManager& texture_mgr
    );
    ~VulkanAssetTaskManager();
    VulkanAssetTaskManager(const VulkanAssetTaskManager&) = delete;
    VulkanAssetTaskManager& operator=(const VulkanAssetTaskManager&) = delete;
    void cancel_threads();
        //  Enqueues a load model job for worker threads to complete
    void load_model(uint32_t id, const Mesh& mesh);
    //  Enqueues a load model job for worker threads to complete
    virtual void load_model(uint32_t id, const std::string& path) override;
    //  Enqueues a Spine skeleton for worker threads to complete
    virtual void load_spine(
        assets::AssetId id,
        assets::SpineLoadArgs& load_args,
        const assets::TextureCreateArgs& args
    ) override;
    //  Enqueues a load texture job for worker threads to complete
    virtual void load_texture(
        assets::AssetId id,
        assets::TextureLoadArgs& load_args,
        const assets::TextureCreateArgs& create_args
    ) override;
    void shutdown();
    void start_threads();
};
}
