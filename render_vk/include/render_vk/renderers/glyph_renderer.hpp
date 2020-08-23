#pragma once

#include "render/glyph_batch.hpp"
#include "render_vk/frame_objects.hpp"
#include "render_vk/vulkan.hpp"
#include <functional>

namespace render_vk
{
struct DescriptorSetLayouts;
class ModelManager;
struct VulkanSwapchain;

//  Draws 2D sprites.
class GlyphRenderer
{
    VkDevice m_device                   {VK_NULL_HANDLE};
    VkRenderPass m_render_pass          {VK_NULL_HANDLE};

    VkPipelineLayout m_pipeline_layout  {VK_NULL_HANDLE};
    VkPipeline m_pipeline               {VK_NULL_HANDLE};

    ModelManager& m_model_mgr;

public:
    GlyphRenderer(ModelManager& model_mgr);
    GlyphRenderer(const GlyphRenderer&) = delete;
    GlyphRenderer& operator=(const GlyphRenderer&) = delete;
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
    //  Draws 2D glyphs
    void draw_glyphs(
        const std::vector<render::GlyphBatch>& batches,
        const FrameDescriptorObjects& descriptors,
        VkCommandBuffer command_buffer
    );
};
}
