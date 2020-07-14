#pragma once

#include "render/renderer.hpp"
#include "render_vk/command_buffer.hpp"
#include "render_vk/dynamic_uniform_buffer.hpp"
#include "render_vk/render_job_manager.hpp"
#include "render_vk/texture.hpp"
#include "render_vk/ubo.hpp"
#include "render_vk/uniform_buffer.hpp"
#include "render_vk/vulkan_queue.hpp"
#include "render_vk/vulkan_swapchain.hpp"
#include "render_vk/vulkan.hpp"
#include <vector>

namespace render
{
class Model;
}

namespace render_vk
{
class ModelManager;

class VulkanRenderer : public render::Renderer
{
    bool m_framebuffer_resized;
    size_t m_current_frame;

    VkInstance m_instance;
    VkDevice m_device;
    VkPhysicalDevice m_physical_device;

    //  Queues
    VkQueue m_graphics_queue;
    VkQueue m_present_queue;

    VkSurfaceKHR m_surface;
    VulkanSwapchain m_swapchain;
    VkRenderPass m_render_pass;

    //  Pipeline objects
    VkPipelineLayout m_pipeline_layout;
    VkPipeline m_graphics_pipeline;

    //  Command objects
    VkCommandPool m_command_pool;
    std::vector<VkCommandBuffer> m_command_buffers;

    //  Shader objects
    VkDescriptorSetLayout m_descriptor_set_layout;
    std::vector<VkDescriptorSet> m_descriptor_sets;
    VkDescriptorPool m_descriptor_pool;

    //  Frame sync objects
    std::vector<VkSemaphore> m_image_available_semaphores;
    std::vector<VkSemaphore> m_render_finished_semaphores;
    std::vector<VkFence> m_in_flight_fences;
    std::vector<VkFence> m_images_in_flight;

    //  Texture objects
    std::vector<Texture> m_textures;

    //  Depth buffer objects
    VkImage m_depth_image;
    VkDeviceMemory m_depth_image_memory;
    VkImageView m_depth_image_view;

    std::unique_ptr<ModelManager> m_model_mgr;

    //  Draw commands for this frame
    std::vector<DrawModelCommand> m_draw_model_commands;

    //  Per-frame uniform buffer
    UniformBuffer<FrameUbo> m_frame_uniform;

    //  Per-object dynamic uniform buffer
    DynamicUniformBuffer<ObjectUbo> m_object_uniform;

    VulkanQueue m_queue;

    RenderJobManager m_job_mgr;

    void create_descriptor_sets();

    //  Creates swapchain and render pass
    void create_swapchain_objects(GLFWwindow* glfw_window);

    //  Creates objects that require the swapchain to exist before they
    //  can be created (and need to be recreated when the swapchain changes).
    void create_swapchain_dependents();

    void cleanup_swapchain();
    void recreate_swapchain(GLFWwindow* glfw_window);
    void update_uniform_buffers(uint32_t image_index);

public:
    VulkanRenderer();
    ~VulkanRenderer();

    virtual void begin_frame() override;
    //  TODO: Change parameters to single MVP object (rename UniformBufferObject struct)
    virtual void draw_frame(GLFWwindow* glfw_window) override;
    virtual void draw_model(
        const common::AssetId model_id,
        const common::AssetId texture_id,
        const glm::mat4x4& model,
        const glm::mat4x4& view,
        const glm::mat4x4& proj
    ) override;

    virtual float get_aspect_ratio() const override;
    virtual bool initialize(GLFWwindow* glfw_window) override;

    VkInstance get_instance() {
        return m_instance;
    }

    virtual void load_model(
        common::AssetId id,
        const std::string& path
    ) override;

    virtual void load_texture(
        common::AssetId id,
        const std::string& path
    ) override;

    virtual void resize(GLFWwindow* glfw_window) override;
    virtual void shutdown() override;
};
}
