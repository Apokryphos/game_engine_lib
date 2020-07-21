#pragma once

#include "render/model_batch.hpp"
#include "render/renderer.hpp"
#include "render_vk/descriptor_sets.hpp"
#include "render_vk/depth.hpp"
#include "render_vk/dynamic_uniform_buffer.hpp"
#include "render_vk/uniform_buffer.hpp"
#include "render_vk/vulkan.hpp"
#include "render_vk/vulkan_swapchain.hpp"
#include <map>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

struct GLFWwindow;

namespace render_vk
{
class ModelManager;

class VulkanRenderSystem : public render::Renderer
{
public:
    enum class FrameTaskId
    {
        DrawModels,
        UpdateFrameUniforms,
        UpdateObjectUniforms,
    };

    struct FrameDescriptorObjects
    {
        //  The descriptor pool for this frame.
        VkDescriptorPool pool      = VK_NULL_HANDLE;
        //  The per-frame descriptor set for this frame.
        VkDescriptorSet frame_set  = VK_NULL_HANDLE;
        //  The per-object descriptor set for this frame.
        // VkDescriptorSet object_set = VK_NULL_HANDLE;
        //  The texture array descriptor set for this frame.
        VkDescriptorSet texture_set = VK_NULL_HANDLE;
    };

    struct FrameCommandObjects
    {
        //  The command pool for this frame.
        VkCommandPool pool       = VK_NULL_HANDLE;
        //  The secondary command buffer for this frame.
        VkCommandBuffer buffer   = VK_NULL_HANDLE;
    };

    struct FrameSyncObjects
    {
        //  Swapchain image is acquired.
        VkSemaphore image_acquired       = VK_NULL_HANDLE;
        //  Command buffers are finished processing.
        VkSemaphore present_ready        = VK_NULL_HANDLE;
        //  This frame is complete and resources can be reused.
        VkFence frame_complete           = VK_NULL_HANDLE;
    };

    //  Frame objects for main thread
    struct Frame
    {
        FrameSyncObjects sync;
        FrameCommandObjects command;
    };

    //  Frame objects for worker threads
    struct ThreadFrame
    {
        FrameCommandObjects command;
        FrameDescriptorObjects descriptor;
    };

    struct Job
    {
        FrameTaskId task_id;
        void* args;

        glm::mat4 view;
        glm::mat4 proj;
        std::vector<render::ModelBatch> batches;
    };

    enum class FrameStatus
    {
        None,
        //  Waiting for render threads to complete.
        Busy,
        //  Render threads are complete.
        Ready,
        //  Frame was discarded due to swapchain recreation or worker thread
        //  failure.
        Discarded,
    };

    class RenderTasks
    {
        struct TaskResults
        {
            uint32_t count;
            std::vector<VkCommandBuffer> command_buffers;
        };

        std::map<FrameTaskId, TaskResults> m_results;

    public:
        void add_results(FrameTaskId task_id, VkCommandBuffer command_buffer) {
            TaskResults& results = m_results[task_id];
            ++results.count;
            results.command_buffers.push_back(command_buffer);
        }

        void clear() {
            m_results.clear();
        }

        const std::vector<VkCommandBuffer>& get_command_buffers(
            FrameTaskId task_id
        ) const {
            return m_results.at(task_id).command_buffers;
        }

        uint32_t get_count(FrameTaskId task_id) const {
            if (m_results.find(task_id) != m_results.end()) {
                return m_results.at(task_id).count;
            }

            return 0;
        }
    };

private:
    //  Status of current frame.
    FrameStatus m_frame_status          = FrameStatus::None;

    //  Cancel worker threads when true.
    bool m_cancel_threads               = false;
    //  Recreate swapchain objects when true.
    bool m_framebuffer_resized          = false;

    //  The total number of frames (resources).
    uint8_t m_frame_count               = 3;
    //  The current frame number (0...frame count).
    uint8_t m_current_frame             = 0;
    //  Number of worker threads.
    uint8_t m_thread_count              = 4;
    //  Swapchain image index
    uint32_t m_image_index              = 0;

    VkInstance m_instance               = VK_NULL_HANDLE;
    VkPhysicalDevice m_physical_device  = VK_NULL_HANDLE;
    VkDevice m_device                   = VK_NULL_HANDLE;
    VkSurfaceKHR m_surface              = VK_NULL_HANDLE;
    //  Presentation queue
    VkQueue m_present_queue             = VK_NULL_HANDLE;

    VkRenderPass m_render_pass          = VK_NULL_HANDLE;
    VkPipelineLayout m_pipeline_layout  = VK_NULL_HANDLE;
    VkPipeline m_graphics_pipeline      = VK_NULL_HANDLE;

    //  Command pool used to create main thread resources
    VkCommandPool m_resource_command_pool = VK_NULL_HANDLE;

    DescriptorSetLayouts m_descriptor_set_layouts;
    //  Debug messenger
    VkDebugUtilsMessengerEXT m_debug_messenger    = VK_NULL_HANDLE;

    GLFWwindow* m_glfw_window = nullptr;

    //  Graphics queue
    VulkanQueue m_graphics_queue;
    //  Swapchain
    VulkanSwapchain m_swapchain;
    //  Depth buffer image
    DepthImage m_depth_image;
    //  Per-frame uniform buffer
    UniformBuffer<FrameUbo> m_frame_uniform;
    //  Per-object dynamic uniform buffer
    DynamicUniformBuffer<ObjectUbo> m_object_uniform;

    //  Main thread frame objects
    std::vector<Frame> m_frames;

    //  Worker threads
    std::vector<std::thread> m_threads;

    //  Job queue mutex
    std::mutex m_jobs_mutex;
    //  Job queue
    std::queue<Job> m_jobs;

    std::mutex m_tasks_mutex;
    //  Worker thread secondary commands output
    RenderTasks m_tasks;

    std::unique_ptr<ModelManager> m_model_mgr;

    //  Stops worker threads.
    void cancel_threads();
    //  Creates frame objects.
    void create_frame_resources();
    //  Creates swapchain and render pass.
    void create_swapchain_objects();
    void create_swapchain_dependents();
    void destroy_swapchain();
    void destroy_frame_resources();
    //  Checks if a worker thread job is available.
    bool get_job(Job& job);
    //  Called by worker threads when work is completed.
    void post_work(FrameTaskId task_id, VkCommandBuffer comand_buffer);
    //  Recreates swapchain and dependent objects.
    void recreate_swapchain();
    //  Releases objects.
    void shutdown();
    //  Starts worker threads.
    void start_threads();
    //  Worker thread main function.
    void thread_main(uint8_t thread_id);
    void thread_update_frame_uniforms(
        const glm::mat4& view,
        const glm::mat4& proj
    );
    void thread_update_object_uniforms(
        const std::vector<render::ModelBatch>& batches
    );

public:
    VulkanRenderSystem();
    ~VulkanRenderSystem();
    //  Starts a new frame.
    virtual void begin_frame() override;
    virtual void draw_models(
        std::vector<render::ModelBatch>& batches
    ) override;
    //  Presents the completed frame.
    virtual void end_frame() override;
    virtual float get_aspect_ratio() const override;

    VkInstance get_instance() const {
        return m_instance;
    }

    //  Initializes objects.
    virtual bool initialize(GLFWwindow* glfw_window) override;
    virtual void load_model(
        common::AssetId id,
        const std::string& path
    ) override;
    virtual void load_texture(
        common::AssetId id,
        const std::string& path
    ) override;
    //  Framebuffer was resized
    virtual void resize() override;
    virtual void update_frame_uniforms(
        const glm::mat4& view,
        const glm::mat4& proj
    ) override;
};
}
