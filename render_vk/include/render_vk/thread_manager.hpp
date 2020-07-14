#pragma once

#include "common/asset.hpp"
#include "render_vk/texture.hpp"
#include "render_vk/vulkan.hpp"
#include <cstdint>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <vector>

namespace render_vk
{
class ModelManager;
class VulkanModel;
class VulkanQueue;

class ThreadManager
{
    //  Worker thread state
    struct WorkState
    {
        uint8_t id;

        VkPhysicalDevice physical_device;
        VkDevice device;
        VulkanQueue* graphics_queue;

        VkCommandPool command_pool;
        VkCommandBuffer command_buffer;
    };

    enum class JobType
    {
        LoadModel,
        LoadTexture,
    };

    struct Job
    {
        JobType type;
        void* data;
    };

    struct LoadModelData
    {
        common::AssetId id;
        std::string path;
    };

    struct LoadTextureData
    {
        common::AssetId id;
        std::string path;
    };

    bool m_threads_quit;

    std::mutex m_jobs_mutex;
    std::queue<Job> m_jobs;
    std::vector<std::thread> m_threads;

    ModelManager* m_model_mgr;

    friend std::unique_ptr<VulkanModel> work_load_model(
        const WorkState& state,
        const LoadModelData& data
    );

    friend Texture work_load_texture(
        const WorkState& state,
        const LoadTextureData& data
    );

    bool get_job(Job& job);

    template <typename T>
    void queue_job(JobType type, const T& data) {
        //  Create job and copy data
        Job job{};
        job.type = type;
        job.data = new T(data);

        //  Enqueue job
        std::lock_guard<std::mutex> lock(m_jobs_mutex);
        m_jobs.push(job);
    }

    void work(WorkState state);

public:
    ThreadManager();
    ~ThreadManager();
    void initialize(
        VkPhysicalDevice physical_device,
        VkDevice device,
        VulkanQueue& queue,
        ModelManager& model_mgr
    );
    void load_model(common::AssetId id, const std::string& path);
    void load_texture(common::AssetId id, const std::string& path);
    void shutdown();
    void update();
};
}
