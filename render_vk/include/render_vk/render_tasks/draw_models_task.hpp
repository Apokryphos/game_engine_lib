#pragma once

#include "render/renderers/model_renderer.hpp"
#include "render_vk/descriptor_sets.hpp"
#include "render_vk/render_job_manager.hpp"
#include "render_vk/vulkan.hpp"
#include <glm/vec3.hpp>
#include <cstdint>
#include <string>

namespace render_vk
{
struct DrawModelsArgs
{
    //  Current swapchain image index
    uint32_t current_frame;
    uint32_t current_image;
    uint32_t object_uniform_align;
    VkRenderPass render_pass;
    VkPipelineLayout pipeline_layout;
    VkPipeline graphics_pipeline;
    DescriptorSets* descriptor_sets;
    std::vector<render::ModelBatch> batches;
};

RenderJobResult task_draw_models(RenderThreadState& state, DrawModelsArgs& args);
}
