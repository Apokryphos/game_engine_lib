#pragma once

#include "render/spine_sprite_batch.hpp"
#include "render_vk/dynamic_uniform_buffer.hpp"
#include "render_vk/frame_objects.hpp"
#include "render_vk/vulkan.hpp"
#include <functional>

namespace render_vk
{
struct DescriptorSetLayouts;
class VulkanSpineManager;
struct VulkanSwapchain;

//  Draws 2D Spine sprites.
class SpineSpriteRenderer
{
    VkDevice m_device                   {VK_NULL_HANDLE};
    VkRenderPass m_render_pass          {VK_NULL_HANDLE};

    VkPipelineLayout m_pipeline_layout  {VK_NULL_HANDLE};
    VkPipeline m_pipeline               {VK_NULL_HANDLE};

    //  Per-object dynamic uniform buffer
    DynamicUniformBuffer<SpineUbo>& m_spine_uniform;

    VulkanSpineManager& m_spine_mgr;

public:
    SpineSpriteRenderer(
        DynamicUniformBuffer<SpineUbo>& spine_uniform,
        VulkanSpineManager& model_mgr
    );
    SpineSpriteRenderer(const SpineSpriteRenderer&) = delete;
    SpineSpriteRenderer& operator=(const SpineSpriteRenderer&) = delete;
    //  Creates resources
    void create_objects(
        VkPhysicalDevice physical_device,
        VkDevice device,
        const VulkanSwapchain& swapchain,
        VkRenderPass render_pass,
        const VkSampleCountFlagBits msaa_sample_count,
        const DescriptorSetLayouts& descriptor_set_layouts
    );
    //  Destroys resources
    void destroy_objects();
    //  Draws 2D Spine sprites
    void draw_sprites(
        const std::vector<render::SpineSpriteBatch>& batches,
        const FrameDescriptorObjects& descriptors,
        VkCommandBuffer command_buffer
    );
    void update_object_uniforms(
        const std::vector<render::SpineSpriteBatch>& batches
    );
};
}
