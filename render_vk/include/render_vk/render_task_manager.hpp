#pragma once

#include "common/log.hpp"
#include "common/job_queue.hpp"
#include "render/glyph_batch.hpp"
#include "render/model_batch.hpp"
#include "render/sprite_batch.hpp"
#include "render/spine_sprite_batch.hpp"
#include "render_vk/frame_objects.hpp"
#include "render_vk/ubo.hpp"
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
class GlyphRenderer;
class ModelManager;
class ModelRenderer;
class SpineSpriteRenderer;
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
        DrawGlyphs,
        DrawModels,
        DrawSpines,
        DrawSprites,
        UpdateFrameUniforms,
        UpdateGlyphUniforms,
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
        std::vector<render::GlyphBatch> glyph_batches;
        std::vector<render::ModelBatch> batches;
        std::vector<render::SpriteBatch> sprite_batches;
        std::vector<render::SpineSpriteBatch> spine_batches;

        uint32_t instance_count {0};

        render::GlyphBatch glyph_batch;
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
        mutable std::mutex m_mutex;
        std::map<TaskId, TaskResults> m_results;

        //  Secondary command buffers generated by worker threads
        //  stored in the order they were started.
        std::map<uint32_t, VkCommandBuffer> m_command_buffers;

    public:
        //  Returns the order of this call
        uint32_t add_call(TaskId task_id) {
            std::lock_guard<std::mutex> lock(m_mutex);

            TaskResults& results = m_results[task_id];
            ++results.called;

            return m_order++;
        }

        void add_results(
            TaskId task_id,
            uint32_t order,
            VkCommandBuffer command_buffer
        ) {
            std::lock_guard<std::mutex> lock(m_mutex);

            TaskResults& results = m_results[task_id];

            //  Only add command buffer if the task actually generates one.
            switch (task_id) {
                default:
                    throw std::runtime_error("Not implemented.");

                //  Tasks that do not generate secondary command buffers
                case TaskId::UpdateFrameUniforms:
                case TaskId::UpdateGlyphUniforms:
                case TaskId::UpdateObjectUniforms:
                    break;

                //  Tasks that generate secondary command buffers
                case TaskId::DrawBillboards:
                case TaskId::DrawGlyphs:
                case TaskId::DrawModels:
                case TaskId::DrawSpines:
                case TaskId::DrawSprites:
                    m_command_buffers[order] = command_buffer;
                    break;
            }

            ++results.complete;
        }

        void clear() {
            std::lock_guard<std::mutex> lock(m_mutex);

            m_order = 0;
            m_results.clear();
            m_command_buffers.clear();
        }

        void get_command_buffers(
            std::vector<VkCommandBuffer>& command_buffers
        ) const {
            std::lock_guard<std::mutex> lock(m_mutex);

            for (const auto& pair : m_command_buffers) {
                command_buffers.push_back(pair.second);
            }
        }

        bool is_complete() const {
            std::lock_guard<std::mutex> lock(m_mutex);

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
    uint8_t m_frame_count  {0};
    //  Number of worker threads.
    uint8_t m_thread_count {4};

    //  The current frame number (0...frame count).
    uint8_t m_current_frame {0};

    //  The cumulative frame number (0...UINT32_MAX)
    uint32_t m_cumulative_frame {0};

    uint32_t m_max_objects {0};

    VkPhysicalDevice m_physical_device {VK_NULL_HANDLE};
    VkDevice m_device                  {VK_NULL_HANDLE};

    //  Worker threads
    std::vector<std::thread> m_threads;

    //  Job queue
    common::JobQueue<Job> m_jobs;

    //  Worker thread task results
    RenderTasks m_tasks;

    DescriptorSetLayouts& m_descriptor_set_layouts;
    DescriptorSetManager& m_descriptor_set_mgr;
    ModelManager& m_model_mgr;
    TextureManager& m_texture_mgr;

    std::vector<FrameUniformObjects> m_uniform_buffers;

    //  Adds a new job for a worker thread to process.
    void add_job(Job& job);
    //  Called by worker threads when work is completed.
    void post_results(
        TaskId task_id,
        uint32_t order,
        VkCommandBuffer comand_buffer
    );
    void thread_main(uint8_t thread_id);
    void update_glyph_uniforms(render::GlyphBatch&& glyph_batch);

public:
    RenderTaskManager(
        VkPhysicalDevice physical_device,
        VkDevice device,
        DescriptorSetLayouts& descriptor_set_layouts,
        DescriptorSetManager& descriptor_set_mgr,
        ModelManager& model_mgr,
        TextureManager& texture_mgr,
        uint8_t frame_count,
        uint32_t max_objects
    );
    ~RenderTaskManager();
    RenderTaskManager(const RenderTaskManager&) = delete;
    RenderTaskManager& operator=(const RenderTaskManager&) = delete;
    void begin_frame(
        uint32_t cumulative_frame,
        uint8_t current_frame,
        bool discard_frame
    );
    void cancel_threads();
    bool check_tasks_complete();
    void draw_billboards(
        BillboardRenderer& renderer,
        const std::vector<render::SpriteBatch>& batches
    );
    void draw_glyphs(
        GlyphRenderer& renderer,
        render::GlyphBatch& glyph_batch
    );
    void draw_models(
        ModelRenderer& renderer,
        const std::vector<render::ModelBatch>& batches
    );
    void draw_spines(
        SpineSpriteRenderer& renderer,
        const std::vector<render::SpineSpriteBatch>& batches
    );
    void draw_sprites(
        SpriteRenderer& renderer,
        const std::vector<render::SpriteBatch>& batches
    );
    void end_frame();
    void get_command_buffers(std::vector<VkCommandBuffer>& command_buffers);
    void shutdown();
    void start_threads();
    void update_frame_uniforms(
        const glm::mat4& view,
        const glm::mat4& proj,
        const glm::mat4& ortho_view,
        const glm::mat4& ortho_proj
    );
};
}
