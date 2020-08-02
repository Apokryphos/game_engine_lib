#pragma once

#include "render/sprite_batch.hpp"
#include "render_vk/frame_objects.hpp"
#include "render_vk/vulkan.hpp"
#include <functional>

namespace render_vk
{
struct DescriptorSetLayouts;
class ModelManager;
struct VulkanSwapchain;

//  Draws 3D billboard sprites.
class BillboardRenderer
{
    VkDevice m_device                   {VK_NULL_HANDLE};
    VkRenderPass m_render_pass          {VK_NULL_HANDLE};

    VkPipelineLayout m_pipeline_layout  {VK_NULL_HANDLE};
    VkPipeline m_pipeline               {VK_NULL_HANDLE};

    ModelManager& m_model_mgr;

public:
    BillboardRenderer(ModelManager& model_mgr);
    BillboardRenderer(const BillboardRenderer&) = delete;
    BillboardRenderer& operator=(const BillboardRenderer&) = delete;
    //  Creates resources
    void create_objects(
        VkDevice device,
        const VulkanSwapchain& swapchain,
        VkRenderPass render_pass,
        const VkSampleCountFlagBits msaa_sample_count,
        const DescriptorSetLayouts& descriptor_set_layouts
    );
    //  Destroys resources
    void destroy_objects();
    //  Draws 3D billboard sprites
    void draw_billboards(
        const std::vector<render::SpriteBatch>& batches,
        const FrameDescriptorObjects& descriptors,
        VkCommandBuffer command_buffer
    );
};
}
