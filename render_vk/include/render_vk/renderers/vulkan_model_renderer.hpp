#pragma once

#include "render/model_batch.hpp"
#include "render_vk/vulkan.hpp"
#include "render_vk/vulkan_render_system.hpp"
#include <functional>

namespace render_vk
{
struct DescriptorSetLayouts;
class ModelManager;
struct VulkanSwapchain;

class VulkanModelRenderer
{
    VkDevice m_device                   {VK_NULL_HANDLE};
    VkRenderPass m_render_pass          {VK_NULL_HANDLE};

    VkPipelineLayout m_pipeline_layout  {VK_NULL_HANDLE};
    VkPipeline m_pipeline               {VK_NULL_HANDLE};

    ModelManager& m_model_mgr;

public:
    VulkanModelRenderer(ModelManager& model_mgr);
    VulkanModelRenderer(const VulkanModelRenderer&) = delete;
    VulkanModelRenderer& operator=(const VulkanModelRenderer&) = delete;
    //  Creates resources
    void create_objects(
        VkDevice device,
        const VulkanSwapchain& swapchain,
        VkRenderPass render_pass,
        const DescriptorSetLayouts& descriptor_set_layouts
    );
    //  Destroys resources
    void destroy_objects();
    //  Draws 3D models
    void draw_models(
        const std::vector<render::ModelBatch>& batches,
        const VulkanRenderSystem::FrameDescriptorObjects& descriptors,
        VkCommandBuffer command_buffer
    );
};
}
