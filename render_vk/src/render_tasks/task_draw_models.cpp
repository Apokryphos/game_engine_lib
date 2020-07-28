#include "common/log.hpp"
#include "render_vk/debug_utils.hpp"
#include "render_vk/model_manager.hpp"
#include "render_vk/render_tasks/task_draw_models.hpp"
#include "render_vk/vulkan_model.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <string>

using namespace common;
using namespace render;

namespace render_vk
{
//  ----------------------------------------------------------------------------
void task_draw_models(
    const VkRenderPass render_pass,
    const VkPipelineLayout pipeline_layout,
    const VkPipeline graphics_pipeline,
    ModelManager& model_mgr,
    const VulkanRenderSystem::FrameDescriptorObjects& descriptors,
    const std::vector<render::ModelBatch>& batches,
    VkCommandBuffer command_buffer
) {
    VkCommandBufferInheritanceInfo inherit_info{};
    inherit_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
    inherit_info.renderPass = render_pass;

    //  Record secondary command buffer
    VkCommandBufferBeginInfo begin_info{};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags =
        VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT |
        VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
    begin_info.pInheritanceInfo = &inherit_info;

    begin_debug_marker(command_buffer, "Draw Models", DEBUG_MARKER_COLOR_ORANGE);
    if (vkBeginCommandBuffer(command_buffer, &begin_info) != VK_SUCCESS) {
        throw std::runtime_error("Failed to begin recording command buffer.");
    }

    //  Check if batches are empty
    if (batches.empty()) {
        if (vkEndCommandBuffer(command_buffer) != VK_SUCCESS) {
            throw std::runtime_error("Failed to record secondary command buffer.");
        }
        end_debug_marker(command_buffer);
        return;
    }

    //  Bind pipeline
    vkCmdBindPipeline(
        command_buffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        graphics_pipeline
    );

    //  Bind per-frame descriptors
    vkCmdBindDescriptorSets(
        command_buffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        pipeline_layout,
        0,
        1,
        &descriptors.frame_set,
        0,
        nullptr
    );

    //  Bind texture descriptors
    vkCmdBindDescriptorSets(
        command_buffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        pipeline_layout,
        1,
        1,
        &descriptors.texture_set,
        0,
        nullptr
    );

    //  Keep track of model index because of dynamic buffer alignment
    for (const ModelBatch& batch : batches) {
        //  Get model
        VulkanModel* model = model_mgr.get_model(batch.model_id);
        if (model == nullptr) {
            continue;
        }

        //  Bind vertex buffer
        VkBuffer vertex_buffers[] = { model->get_vertex_buffer() };
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(command_buffer, 0, 1, vertex_buffers, offsets);

        //  Bind index buffer
        vkCmdBindIndexBuffer(
            command_buffer,
            model->get_index_buffer(),
            0,
            VK_INDEX_TYPE_UINT32
        );

        // const uint32_t dynamic_align = static_cast<uint32_t>(m_object_uniform.get_align());
        const uint32_t index_count = model->get_index_count();

        // vkCmdBindDescriptorSets(
        //     command_buffer,
        //     VK_PIPELINE_BIND_POINT_GRAPHICS,
        //     m_pipeline_layout,
        //     1,
        //     1,
        //     &descriptor.object_set,
        //     1,
        //     &dynamic_align
        // );

        //  Texture ID
        vkCmdPushConstants(
            command_buffer,
            pipeline_layout,
            VK_SHADER_STAGE_FRAGMENT_BIT,
            sizeof(glm::mat4),
            sizeof(uint32_t),
            &batch.texture_id
        );

        //  Draw each object
        for (uint32_t n = 0; n < batch.positions.size(); ++n) {
            //  One dynamic offset per dynamic descriptor to offset into the ubo
            //  containing all model matrices
            // const uint32_t dynamic_offset = n * dynamic_align;

            glm::mat4 model = glm::translate(glm::mat4(1.0f), batch.positions[n]);

            vkCmdPushConstants(
                command_buffer,
                pipeline_layout,
                VK_SHADER_STAGE_VERTEX_BIT,
                0,
                sizeof(glm::mat4),
                &model
            );

            //  Draw
            vkCmdDrawIndexed(
                command_buffer,
                index_count,
                1,
                0,
                0,
                1
            );
        }
    }

    if (vkEndCommandBuffer(command_buffer) != VK_SUCCESS) {
        throw std::runtime_error("Failed to record secondary command buffer.");
    }

    end_debug_marker(command_buffer);
}
}
