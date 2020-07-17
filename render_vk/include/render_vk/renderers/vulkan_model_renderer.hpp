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
    VkDevice m_device;

    //  Draw commands
    //  TODO: Replace with secondary command buffer
    std::vector<DrawModelCommand> m_draw_commands;

    //  Per-frame uniform buffer
    UniformBuffer<FrameUbo> m_frame_uniform;

    //  Per-object dynamic uniform buffer
    DynamicUniformBuffer<ObjectUbo> m_object_uniform;

    ModelManager& m_model_mgr;

    void update_uniform_buffers();

public:
    VulkanModelRenderer(ModelManager& model_mgr);

    void begin_frame() {
        m_draw_commands.clear();
    }

    void create_objects(VkPhysicalDevice physical_device, VkDevice device);
    void destroy_objects();

    virtual void draw_models(
        const glm::mat4& view,
        const glm::mat4& proj,
        std::vector<uint32_t>& model_ids,
        std::vector<glm::vec3>& positions,
        std::vector<uint32_t>& texture_ids
    ) override;

    const std::vector<DrawModelCommand>& get_draw_commands() const {
        return m_draw_commands;
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
