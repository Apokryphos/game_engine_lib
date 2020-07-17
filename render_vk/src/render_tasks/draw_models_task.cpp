#include "common/log.hpp"
#include "common/thread_manager.hpp"
#include "render_vk/debug_utils.hpp"
#include "render_vk/mesh.hpp"
#include "render_vk/model_manager.hpp"
#include "render_vk/renderers/vulkan_model_renderer.hpp"
#include "render_vk/render_tasks/draw_models_task.hpp"
#include "render_vk/render_tasks/render_task_ids.hpp"
#include "render_vk/vulkan_model.hpp"
#include "render_vk/vulkan_renderer.hpp"
#include <string>

using namespace common;
using namespace render;

namespace render_vk
{
//  ----------------------------------------------------------------------------
RenderJobResult task_draw_models(RenderThreadState& state, DrawModelsArgs& args) {
    //  Build secondary command buffer
    VkCommandBuffer command_buffer = state.command_buffers.at(args.current_image);

    VkCommandBufferInheritanceInfo inherit_info{};
    inherit_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
    inherit_info.renderPass = args.render_pass;

    //  Record secondary command buffer
    VkCommandBufferBeginInfo begin_info{};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags = VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
    begin_info.pInheritanceInfo = &inherit_info;

    if (vkBeginCommandBuffer(command_buffer, &begin_info) != VK_SUCCESS) {
        throw std::runtime_error("Failed to begin recording command buffer.");
    }

    //  Bind pipeline
    vkCmdBindPipeline(
        command_buffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        args.graphics_pipeline
    );

    begin_debug_marker(command_buffer, "Draw Model (SECONDARY)", DEBUG_MARKER_COLOR_ORANGE);

    //  Keep track of model index because of dynamic buffer alignment
    size_t model_index = 0;

    for (const ModelBatch& batch : args.batches) {
        //  Get model
        VulkanModel* model = state.model_mgr->get_model(batch.model_id);
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

        //  Bind per-frame descriptors
        vkCmdBindDescriptorSets(
            command_buffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            args.pipeline_layout,
            0,
            1,
            &args.descriptor_sets->frame_sets.at(args.current_image),
            0,
            nullptr
        );

        //  Draw each object
        for (int n = 0; n < batch.positions.size(); ++n) {
            //  One dynamic offset per dynamic descriptor to offset into the ubo
            //  containing all model matrices
            const size_t dynamic_align = args.object_uniform_align;
            const uint32_t dynamic_offset = model_index * static_cast<uint32_t>(dynamic_align);

            //  Bind per-object descriptor set using the dynamic offset
            vkCmdBindDescriptorSets(
                command_buffer,
                VK_PIPELINE_BIND_POINT_GRAPHICS,
                args.pipeline_layout,
                1,
                1,
                &args.descriptor_sets->object_sets.at(args.current_image),
                1,
                &dynamic_offset
            );

            ++model_index;

            //  Draw
            vkCmdDrawIndexed(
                command_buffer,
                static_cast<uint32_t>(model->get_index_count()),
                1,
                0,
                0,
                0
            );
        }
    }

    end_debug_marker(command_buffer);

    if (vkEndCommandBuffer(command_buffer) != VK_SUCCESS) {
        throw std::runtime_error("Failed to record secondary command buffer.");
    }

    VkFence complete_fence;
    VkFenceCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    vkCreateFence(state.device, &info, nullptr, &complete_fence);

    RenderJobResult result{};
    result.current_frame = args.current_frame;
    result.complete_fence = complete_fence;

    state.renderer->post_command_buffer(
        args.current_frame,
        RENDER_TASK_DRAW_MODELS,
        command_buffer,
        result.complete_fence
    );

    return result;
}
}
