#pragma once

#include "common/asset.hpp"
#include "common/thread_manager.hpp"
#include "render_vk/vulkan.hpp"
#include "render_vk/vulkan_queue.hpp"
#include <glm/vec3.hpp>

namespace render_vk
{
class ModelManager;

struct RenderThreadState
{
    VkPhysicalDevice physical_device;
    VkDevice device;
    VulkanQueue* graphics_queue;
    VkCommandPool command_pool;
    ModelManager* model_mgr;
};

class RenderJobManager
{
    common::ThreadManager<RenderThreadState> m_thread_mgr;

    void initialize_tasks();

public:
    RenderJobManager();
    ~RenderJobManager();
    RenderJobManager(const RenderJobManager&) = delete;
    RenderJobManager& operator=(const RenderJobManager&) = delete;

    void draw_models(
        const std::vector<uint32_t>& model_ids,
        const std::vector<glm::vec3>& positions
    );

    void initialize(
        VkPhysicalDevice physical_device,
        VkDevice device,
        VulkanQueue& graphics_queue,
        ModelManager& model_mgr
    );

    void load_model(common::AssetId id, const std::string& path);
    void load_texture(common::AssetId id, const std::string& path);
    void shutdown();
};
}
