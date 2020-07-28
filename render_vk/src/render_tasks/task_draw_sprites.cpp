#include "common/log.hpp"
#include "render_vk/debug_utils.hpp"
#include "render_vk/model_manager.hpp"
#include "render_vk/render_tasks/task_draw_sprites.hpp"
#include "render_vk/sprite_pipeline.hpp"
#include "render_vk/vulkan_model.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <string>

using namespace common;
using namespace render;

namespace render_vk
{
//  ----------------------------------------------------------------------------
void task_draw_sprites(
    const VkRenderPass render_pass,
    const SpritePipeline& pipeline,
    ModelManager& model_mgr,
    const VulkanRenderSystem::FrameDescriptorObjects& descriptors,
    const std::vector<render::SpriteBatch>& batches,
    VkCommandBuffer command_buffer
) {
    VkCommandBufferInheritanceInfo inherit_info{};
    inherit_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
    inherit_info.renderPass = render_pass;

    //  Record secondary command buffer
    VkCommandBufferBeginInfo begin_info{};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags = VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
    begin_info.pInheritanceInfo = &inherit_info;

    begin_debug_marker(command_buffer, "Draw Sprites", DEBUG_MARKER_COLOR_ORANGE);
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
        pipeline.pipeline
    );

    //  Bind per-frame descriptors
    vkCmdBindDescriptorSets(
        command_buffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        pipeline.layout,
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
        pipeline.layout,
        1,
        1,
        &descriptors.texture_set,
        0,
        nullptr
    );

    //  Get sprite quad
    VulkanModel& quad = model_mgr.get_quad();

    //  Keep track of model index because of dynamic buffer alignment
    for (const SpriteBatch& batch : batches) {
        //  Bind vertex buffer
        VkBuffer vertex_buffers[] = { quad.get_vertex_buffer() };
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(command_buffer, 0, 1, vertex_buffers, offsets);

        //  Bind index buffer
        vkCmdBindIndexBuffer(
            command_buffer,
            quad.get_index_buffer(),
            0,
            VK_INDEX_TYPE_UINT32
        );

        const uint32_t index_count = quad.get_index_count();

        //  Texture ID
        vkCmdPushConstants(
            command_buffer,
            pipeline.layout,
            VK_SHADER_STAGE_FRAGMENT_BIT,
            sizeof(glm::mat4),
            sizeof(uint32_t),
            &batch.texture_id
        );

        //  Draw each object
        for (uint32_t n = 0; n < batch.positions.size(); ++n) {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), batch.positions[n]);

            vkCmdPushConstants(
                command_buffer,
                pipeline.layout,
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
