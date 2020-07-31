#pragma once

#include "common/log.hpp"
#include "common/job_queue.hpp"
#include "render/model_batch.hpp"
#include "render/sprite_batch.hpp"
#include "render_vk/dynamic_uniform_buffer.hpp"
#include "render_vk/frame_objects.hpp"
#include "render_vk/ubo.hpp"
#include "render_vk/uniform_buffer.hpp"
#include "render_vk/vulkan.hpp"
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <map>
#include <mutex>
#include <queue>
#include <stdexcept>
#include <thread>
#include <vector>

namespace render_vk
{
class DescriptorSetManager;
class BillboardRenderer;
class ModelManager;
class ModelRenderer;
class SpriteRenderer;
class TextureManager;

struct ThreadFrameCommandObjects
{
    //  The command pool for this frame.
    VkCommandPool pool       = VK_NULL_HANDLE;
    //  The secondary command buffers for this frame.
    std::vector<VkCommandBuffer> buffers;
};

class RenderTaskManager
{
    enum class TaskId
    {
        None,
        DrawBillboards,
        DrawModels,
        DrawSprites,
        UpdateFrameUniforms,
        UpdateObjectUniforms,
    };

    static bool task_requires_textures(TaskId task_id);
    static const char* task_id_to_string(TaskId task_id);

    struct Job
    {
        TaskId task_id {TaskId::None};
        uint32_t order {0};
        // void* args     {nullptr};
        void* renderer {nullptr};

        FrameUbo frame_ubo;
        std::vector<render::ModelBatch> batches;
        std::vector<render::SpriteBatch> sprite_batches;
    };

    //  Frame objects for worker threads
    struct ThreadFrame
    {
        uint32_t command_buffer_index {0};
        uint32_t texture_timestamp {0};
        std::string name;
        ThreadFrameCommandObjects command;
        FrameDescriptorObjects descriptor;
    };

    class RenderTasks
    {
        struct TaskResults
        {
            uint32_t called;
            uint32_t complete;
        };

        uint32_t m_order {0};
        std::map<TaskId, TaskResults> m_results;

        //  Secondary command buffers generated by worker threads
        //  stored in the order they were started.
        std::map<uint32_t, VkCommandBuffer> m_command_buffers;

    public:
        //  Returns the order of this call
        uint32_t add_call(TaskId task_id) {
            TaskResults& results = m_results[task_id];
            ++results.called;

            return m_order++;
        }

        void add_results(
            TaskId task_id,
            uint32_t order,
            VkCommandBuffer command_buffer
        ) {
            TaskResults& results = m_results[task_id];

            //  Only add command buffer if the task actually generates one.
            switch (task_id) {
                default:
                    throw std::runtime_error("Not implemented.");

                //  Tasks that do not generate secondary command buffers
                case TaskId::UpdateObjectUniforms:
                case TaskId::UpdateFrameUniforms:
                    break;

                //  Tasks that generate secondary command buffers
                case TaskId::DrawBillboards:
                case TaskId::DrawModels:
                case TaskId::DrawSprites:
                    m_command_buffers[order] = command_buffer;
                    break;
            }

            ++results.complete;
        }

        void clear() {
            m_order = 0;
            m_results.clear();
            m_command_buffers.clear();
        }

        void get_command_buffers(
            std::vector<VkCommandBuffer>& command_buffers
        ) const {
            for (const auto& pair : m_command_buffers) {
                command_buffers.push_back(pair.second);
            }
        }

        bool is_complete() const {
            for (const auto& pair : m_results) {
                if (pair.second.called != pair.second.complete) {
                    return false;
                }
            }

            return true;
        }
    };

    //  True when frame was discarded and draw requests should be ignored.
    bool m_discard_frame  {false};

    //  The total number of frames (resources).
    uint8_t m_frame_count  {3};
    //  Number of worker threads.
    uint8_t m_thread_count {4};

    //  The current frame number (0...frame count).
    uint8_t m_current_frame {0};

    VkPhysicalDevice m_physical_device {VK_NULL_HANDLE};
    VkDevice m_device                  {VK_NULL_HANDLE};

    //  Worker threads
    std::vector<std::thread> m_threads;

    //  Job queue
    common::JobQueue<Job> m_jobs;

    //  Tasks mutex
    std::mutex m_tasks_mutex;
    //  Worker thread task results
    RenderTasks m_tasks;

    DescriptorSetLayouts& m_descriptor_set_layouts;
    //  Per-frame uniform buffer
    UniformBuffer<FrameUbo>& m_frame_uniform;
    //  Per-object dynamic uniform buffer
    DynamicUniformBuffer<ObjectUbo>& m_object_uniform;
    DescriptorSetManager& m_descriptor_set_mgr;
    ModelManager& m_model_mgr;
    TextureManager& m_texture_mgr;

    //  Adds a new job for a worker thread to process.
    void add_job(Job& job);
    //  Called by worker threads when work is completed.
    void post_results(
        TaskId task_id,
        uint32_t order,
        VkCommandBuffer comand_buffer
    );
    void thread_main(uint8_t thread_id);

public:
    RenderTaskManager(
        VkPhysicalDevice physical_device,
        VkDevice device,
        DescriptorSetLayouts& descriptor_set_layouts,
        UniformBuffer<FrameUbo>& frame_uniform,
        DynamicUniformBuffer<ObjectUbo>& object_uniform,
        DescriptorSetManager& descriptor_set_mgr,
        ModelManager& model_mgr,
        TextureManager& texture_mgr
    );
    ~RenderTaskManager();
    RenderTaskManager(const RenderTaskManager&) = delete;
    RenderTaskManager& operator=(const RenderTaskManager&) = delete;
    void begin_frame(uint32_t current_frame, bool discard_frame);
    void cancel_threads();
    bool check_tasks_complete();
    void draw_billboards(
        BillboardRenderer& renderer,
        const std::vector<render::SpriteBatch>& batches
    );
    void draw_models(
        ModelRenderer& renderer,
        const std::vector<render::ModelBatch>& batches
    );
    void draw_sprites(
        SpriteRenderer& renderer,
        const std::vector<render::SpriteBatch>& batches
    );
    void end_frame();
    void get_command_buffers(std::vector<VkCommandBuffer>& command_buffers);
    void start_threads();
    void update_frame_uniforms(
        const glm::mat4& view,
        const glm::mat4& proj,
        const glm::mat4& ortho_view,
        const glm::mat4& ortho_proj
    );
};
}