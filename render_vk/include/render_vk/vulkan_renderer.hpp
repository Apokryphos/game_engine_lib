#pragma once

#include "render/renderer.hpp"
#include "render_vk/command_buffer.hpp"
#include "render_vk/descriptor_sets.hpp"
#include "render_vk/descriptor_set_layout.hpp"
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
class VulkanModelRenderer;

class VulkanRenderer : public render::Renderer
{
    //  True when frame is ready for draw commands
    bool m_frame_ready = false;
    //  True when window was resized and swapchain requires recreation
    bool m_framebuffer_resized = false;
    //  The current frame
    size_t m_current_frame = 0;
    //  The current swapchain image index
    uint32_t m_image_index = 0;

    //  GLFW window
    GLFWwindow* m_glfw_window = nullptr;

    //  Vulkan instance
    VkInstance m_instance = VK_NULL_HANDLE;

    //  Device
    VkDevice m_device = VK_NULL_HANDLE;
    //  Physical device
    VkPhysicalDevice m_physical_device = VK_NULL_HANDLE;

    //  Graphics queue
    VkQueue m_graphics_queue = VK_NULL_HANDLE;
    //  Presentation queue
    VkQueue m_present_queue = VK_NULL_HANDLE;

    VkSurfaceKHR m_surface = VK_NULL_HANDLE;
    VulkanSwapchain m_swapchain;
    VkRenderPass m_render_pass = VK_NULL_HANDLE;

    //  Pipeline objects
    VkPipelineLayout m_pipeline_layout = VK_NULL_HANDLE;
    VkPipeline m_graphics_pipeline = VK_NULL_HANDLE;

    //  Primary command pool
    VkCommandPool m_command_pool = VK_NULL_HANDLE;

    //  Primary command buffers
    std::vector<VkCommandBuffer> m_command_buffers;

    //  Shader objects
    VkDescriptorPool m_descriptor_pool = VK_NULL_HANDLE;
    DescriptorSetLayouts m_descriptor_set_layouts;
    DescriptorSets m_descriptor_sets;

    //  Frame sync objects
    std::vector<VkSemaphore> m_image_available_semaphores;
    std::vector<VkSemaphore> m_render_finished_semaphores;
    std::vector<VkFence> m_in_flight_fences;
    std::vector<VkFence> m_images_in_flight;

    //  Texture objects
    std::vector<Texture> m_textures;

    //  Depth buffer objects
    VkImage m_depth_image = VK_NULL_HANDLE;
    VkDeviceMemory m_depth_image_memory = VK_NULL_HANDLE;
    VkImageView m_depth_image_view = VK_NULL_HANDLE;

    std::unique_ptr<ModelManager> m_model_mgr;

    //  Multithreading objects
    VulkanQueue m_queue;
    RenderJobManager m_job_mgr;

    //  Debug messenger
    VkDebugUtilsMessengerEXT m_debug_messenger = VK_NULL_HANDLE;

    //  Renderers
    std::unique_ptr<VulkanModelRenderer> m_model_renderer;

    void create_descriptor_sets();

    //  Creates swapchain and render pass
    void create_swapchain_objects(GLFWwindow* glfw_window);

    //  Creates objects that require the swapchain to exist before they
    //  can be created (and need to be recreated when the swapchain changes).
    void create_swapchain_dependents();

    void cleanup_swapchain();
    void recreate_swapchain(GLFWwindow* glfw_window);

public:
    VulkanRenderer();
    ~VulkanRenderer();

    virtual void begin_frame() override;
    //  TODO: Change parameters to single MVP object (rename UniformBufferObject struct)
    virtual void draw_frame(GLFWwindow* glfw_window) override;
    virtual float get_aspect_ratio() const override;
    virtual bool initialize(GLFWwindow* glfw_window) override;

    VkInstance get_instance() {
        return m_instance;
    }

    virtual render::ModelRenderer& get_model_renderer() override;

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
