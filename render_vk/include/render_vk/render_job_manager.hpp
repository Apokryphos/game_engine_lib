#pragma once

#include "common/asset.hpp"
#include "common/thread_manager.hpp"
#include "render/renderers/model_renderer.hpp"
#include "render_vk/descriptor_sets.hpp"
#include "render_vk/vulkan.hpp"
#include "render_vk/vulkan_queue.hpp"
#include <glm/vec3.hpp>

namespace render_vk
{
class ModelManager;
class VulkanRenderer;

struct RenderJobResult
{
    uint32_t current_frame = 0;
    VkFence complete_fence = VK_NULL_HANDLE;
};

struct RenderThreadState
{
    uint32_t current_frame;
    VkPhysicalDevice physical_device = VK_NULL_HANDLE;
    VkDevice device = VK_NULL_HANDLE;
    //  Thread-owned command pool
    VkCommandPool command_pool = VK_NULL_HANDLE;
    VulkanQueue* graphics_queue = VK_NULL_HANDLE;
    ModelManager* model_mgr = nullptr;
    VulkanRenderer* renderer = nullptr;
    //  Thread-owned secondary command buffers
    std::vector<VkCommandBuffer> command_buffers;
};

class RenderJobManager
{
    uint32_t m_max_frames_in_flight;
    common::ThreadManager<RenderThreadState, RenderJobResult> m_thread_mgr;

    void initialize_tasks();

public:
    RenderJobManager(const uint32_t max_frames_in_flight);
    ~RenderJobManager();
    RenderJobManager(const RenderJobManager&) = delete;
    RenderJobManager& operator=(const RenderJobManager&) = delete;

    void draw_models(
        VkRenderPass render_pass,
        VkPipelineLayout pipeline_layout,
        VkPipeline graphics_pipeline,
        DescriptorSets& descriptor_sets,
        uint32_t swapchain_image_index,
        uint32_t object_uniform_align,
        const std::vector<render::ModelBatch>& batches
    );

    void initialize(
        VkPhysicalDevice physical_device,
        VkDevice device,
        VulkanQueue& graphics_queue,
        VulkanRenderer& renderer,
        ModelManager& model_mgr,
        uint32_t swapchain_image_count
    );

    void load_model(common::AssetId id, const std::string& path);
    void load_texture(common::AssetId id, const std::string& path);
    void shutdown();
};
}
