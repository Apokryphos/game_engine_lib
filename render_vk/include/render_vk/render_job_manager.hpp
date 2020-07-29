#pragma once

#include "render_vk/vulkan.hpp"
#include <glm/vec3.hpp>

namespace render_vk
{
class DepthImage;
class ModelManager;
class VulkanQueue;
class VulkanSwapchain;

class RenderJobManager
{
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

    void start_threads(
        VkPhysicalDevice physical_device,
        VkDevice device,
        VulkanQueue& graphics_queue,
        VulkanSwapchain& swapchain,
        DepthImage& depth_image
    );
};
}
