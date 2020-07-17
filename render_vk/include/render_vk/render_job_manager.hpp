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
    VkPhysicalDevice physical_device = VK_NULL_HANDLE;
    VkDevice device = VK_NULL_HANDLE;
    //  Thread-owned command pool
    VkCommandPool command_pool = VK_NULL_HANDLE;
    VulkanQueue* graphics_queue = VK_NULL_HANDLE;
    ModelManager* model_mgr = nullptr;
    //  Thread-owned secondary command buffers
    std::vector<VkCommandBuffer> command_buffers;
};

class RenderJobManager
{
    uint32_t m_max_frames_in_flight;
    common::ThreadManager<RenderThreadState> m_thread_mgr;

    void initialize_tasks();

public:
    RenderJobManager(const uint32_t max_frames_in_flight);
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
        ModelManager& model_mgr,
        uint32_t swapchain_image_count
    );

    void load_model(common::AssetId id, const std::string& path);
    void load_texture(common::AssetId id, const std::string& path);
    void shutdown();
};
}
