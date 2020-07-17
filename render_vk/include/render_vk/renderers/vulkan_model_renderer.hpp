#pragma once

#include "render/renderers/model_renderer.hpp"
#include "render_vk/command_buffer.hpp"
#include "render_vk/dynamic_uniform_buffer.hpp"
#include "render_vk/uniform_buffer.hpp"
#include "render_vk/vulkan.hpp"

namespace render_vk
{
class ModelManager;

class VulkanModelRenderer : public render::ModelRenderer
{
    //  Current swapchain image index
    uint32_t m_current_image;

    VkDevice m_device;
    VkRenderPass m_render_pass;
    VkPipelineLayout m_pipeline_layout;
    VkPipeline m_graphics_pipeline;

    //  Set in begin_frame()
    VkDescriptorSet m_descriptor_set;

    VkCommandPool m_command_pool;
    std::vector<VkCommandBuffer> m_command_buffers;

    //  Per-frame uniform buffer
    UniformBuffer<FrameUbo> m_frame_uniform;

    //  Per-object dynamic uniform buffer
    DynamicUniformBuffer<ObjectUbo> m_object_uniform;

    ModelManager& m_model_mgr;

    void update_uniform_buffers();

public:
    VulkanModelRenderer(ModelManager& model_mgr);

    void begin_frame(
        uint32_t current_image,
        VkDescriptorSet descriptor_set
    ) {
        m_current_image = current_image;
        m_descriptor_set = descriptor_set;
    }

    void create_objects(
        VkPhysicalDevice physical_device,
        VkDevice device,
        VkRenderPass render_pass,
        VkPipelineLayout pipeline_layout,
        VkPipeline graphics_pipeline,
        uint32_t swapchain_image_count
    );

    void destroy_objects();

    virtual void draw_models(
        const glm::mat4& view,
        const glm::mat4& proj,
        std::vector<uint32_t>& model_ids,
        std::vector<glm::vec3>& positions,
        std::vector<uint32_t>& texture_ids
    ) override;

    VkCommandBuffer get_command_buffer() const {
        return m_command_buffers.at(m_current_image);
    }

    UniformBuffer<FrameUbo>& get_frame_uniform() {
        return m_frame_uniform;
    }

    DynamicUniformBuffer<ObjectUbo>& get_object_uniform() {
        return m_object_uniform;
    }

    void update_uniform_buffers(
        glm::mat4 view,
        glm::mat4 proj,
        const std::vector<glm::vec3>& positions,
        const std::vector<uint32_t>& texture_ids
    );
};
}
