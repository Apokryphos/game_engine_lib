#include "render_vk/command_buffer.hpp"
#include "render_vk/command_pool.hpp"
#include "render_vk/debug_utils.hpp"
#include "render_vk/model_manager.hpp"
#include "render_vk/renderers/vulkan_model_renderer.hpp"
#include "render_vk/vulkan_model.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <map>

namespace render_vk
{
//  Number of objects to create UBO data structs for
const size_t OBJECT_INSTANCES = 100;

//  ----------------------------------------------------------------------------
VulkanModelRenderer::VulkanModelRenderer(ModelManager& model_mgr)
: m_current_image(0),
  m_descriptor_set(VK_NULL_HANDLE),
  m_model_mgr(model_mgr),
  m_object_uniform(OBJECT_INSTANCES) {
}

//  ----------------------------------------------------------------------------
void VulkanModelRenderer::create_objects(
    VkPhysicalDevice physical_device,
    VkDevice device,
    VkRenderPass render_pass,
    VkPipelineLayout pipeline_layout,
    VkPipeline graphics_pipeline,
    uint32_t swapchain_image_count
) {
    m_device = device;
    m_render_pass = render_pass;
    m_pipeline_layout = pipeline_layout;
    m_graphics_pipeline = graphics_pipeline;

    m_frame_uniform.create(physical_device, m_device);
    m_object_uniform.create(physical_device, m_device);

    create_command_pool(device, physical_device, m_command_pool);

    create_secondary_command_buffers(
        device,
        m_command_pool,
        swapchain_image_count,
        m_command_buffers
    );
}

//  ----------------------------------------------------------------------------
void VulkanModelRenderer::destroy_objects() {
    m_frame_uniform.destroy();
    m_object_uniform.destroy();
    vkDestroyCommandPool(m_device, m_command_pool, nullptr);
    m_device = VK_NULL_HANDLE;
    m_render_pass = VK_NULL_HANDLE;
}

//  ----------------------------------------------------------------------------
void VulkanModelRenderer::draw_models(
    const glm::mat4& view,
    const glm::mat4& proj,
    std::vector<uint32_t>& model_ids,
    std::vector<glm::vec3>& positions,
    std::vector<uint32_t>& texture_ids
) {
    //  Check that descriptor set is valid
    if (m_descriptor_set == VK_NULL_HANDLE) {
        return;
    }

    //  Check that textures exist
    std::vector<Texture> textures;
    m_model_mgr.get_textures(textures);
    if (textures.empty()) {
        return;
    }

    update_uniform_buffers(view, proj, positions, texture_ids);

    //  Build secondary command buffer
    VkCommandBuffer command_buffer = m_command_buffers.at(m_current_image);

    VkCommandBufferInheritanceInfo inherit_info{};
    inherit_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
    inherit_info.renderPass = m_render_pass;

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
        m_graphics_pipeline
    );

    begin_debug_marker(command_buffer, "Draw Model (SECONDARY)", DEBUG_MARKER_COLOR_ORANGE);

    //  Keep track of model index because of dynamic buffer alignment
    size_t model_index = 0;

    //  Draw each model
    const size_t object_count = model_ids.size();
    for (size_t n = 0; n < object_count; ++n) {
        //  Get model
        VulkanModel* model = m_model_mgr.get_model(model_ids[n]);
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

        // One dynamic offset per dynamic descriptor to offset into the ubo containing all model matrices
        const size_t dynamic_align = m_object_uniform.get_align();
        const uint32_t dynamic_offset = model_index * static_cast<uint32_t>(dynamic_align);

        // Bind the descriptor set for rendering a mesh using the dynamic offset
        vkCmdBindDescriptorSets(
            command_buffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            m_pipeline_layout,
            0,
            1,
            &m_descriptor_set,
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

    end_debug_marker(command_buffer);

    if (vkEndCommandBuffer(command_buffer) != VK_SUCCESS) {
        throw std::runtime_error("Failed to record secondary command buffer.");
    }

    m_descriptor_set = VK_NULL_HANDLE;
}

//  ----------------------------------------------------------------------------
void VulkanModelRenderer::update_uniform_buffers(
    glm::mat4 view,
    glm::mat4 proj,
    const std::vector<glm::vec3>& positions,
    const std::vector<uint32_t>& texture_ids
) {
    assert(positions.size() == texture_ids.size());

    //  Update frame UBO
    FrameUbo frame_ubo{};
    frame_ubo.view = view;
    frame_ubo.proj = proj;

    //  GLM (OpenGL) uses inverted Y clip coordinate
    frame_ubo.proj[1][1] *= -1;

    //  Copy frame UBO struct to uniform buffer
    m_frame_uniform.copy(frame_ubo);

    //  Update all UBO structs once per frame
    const size_t object_count = positions.size();
    assert(object_count > 0);

    std::vector<ObjectUbo> data(object_count);
    for (size_t n = 0; n < object_count; ++n)  {
        data[n].texture_index = texture_ids[n];
        data[n].model = glm::translate(glm::mat4(1.0f), positions[n]);
    }

    //  Copy object UBO structs to dynamic uniform buffer
    m_object_uniform.copy(data);
}
}
