#include "common/log.hpp"
#include "common/stopwatch.hpp"
#include "render_vk/command_buffer.hpp"
#include "render_vk/command_pool.hpp"
#include "render_vk/debug_utils.hpp"
#include "render_vk/descriptor_pool.hpp"
#include "render_vk/descriptor_set_layout.hpp"
#include "render_vk/descriptor_set_manager.hpp"
#include "render_vk/model_manager.hpp"
#include "render_vk/render_task_manager.hpp"
#include "render_vk/render_tasks/task_update_uniforms.hpp"
#include "render_vk/renderers/billboard_renderer.hpp"
#include "render_vk/renderers/glyph_renderer.hpp"
#include "render_vk/renderers/model_renderer.hpp"
#include "render_vk/renderers/spine_sprite_renderer.hpp"
#include "render_vk/renderers/sprite_renderer.hpp"
#include "render_vk/texture.hpp"
#include "render_vk/texture_manager.hpp"
#include "render_vk/vulkan_queue.hpp"
#include "render_vk/vulkan_swapchain.hpp"

using namespace common;
using namespace render;

namespace render_vk
{
//  ----------------------------------------------------------------------------
inline void filter_pending_textures(
    const std::vector<GlyphBatch>& batches,
    const TextureManager& texture_mgr,
    std::vector<GlyphBatch>& job_batches
) {
    for (const GlyphBatch& batch : batches) {
        if (texture_mgr.texture_exists(batch.texture_id)) {
            job_batches.push_back(batch);
        }
    }
}

//  ----------------------------------------------------------------------------
inline void filter_pending_textures(
    const std::vector<SpriteBatch>& batches,
    const TextureManager& texture_mgr,
    std::vector<SpriteBatch>& job_batches
) {
    for (const SpriteBatch& batch : batches) {
        if (texture_mgr.texture_exists(batch.texture_id)) {
            job_batches.push_back(batch);
        }
    }
}

//  ----------------------------------------------------------------------------
inline void filter_pending_textures(
    const std::vector<SpineSpriteBatch>& batches,
    const TextureManager& texture_mgr,
    std::vector<SpineSpriteBatch>& job_batches
) {
    for (const SpineSpriteBatch& batch : batches) {
        if (texture_mgr.texture_exists(batch.texture_id)) {
            job_batches.push_back(batch);
        }
    }
}

//  ----------------------------------------------------------------------------
inline void filter_pending_textures(
    const std::vector<ModelBatch>& batches,
    const ModelManager& model_mgr,
    const TextureManager& texture_mgr,
    std::vector<ModelBatch>& job_batches
) {
    for (const ModelBatch& batch : batches) {
        if (model_mgr.model_exists(batch.model_id) &&
            texture_mgr.texture_exists(batch.texture_id)
        ) {
            job_batches.push_back(batch);
        }
    }
}

//  ----------------------------------------------------------------------------
static void create_descriptor_set(
    const VkDevice device,
    const VkDescriptorSetLayout descriptor_set_layout,
    const VkDescriptorPool descriptor_pool,
    const std::string& debug_name,
    VkDescriptorSet& descriptor_set
) {
    VkDescriptorSetAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    alloc_info.descriptorPool = descriptor_pool;
    alloc_info.descriptorSetCount = 1;
    alloc_info.pSetLayouts = &descriptor_set_layout;

    if (vkAllocateDescriptorSets(device, &alloc_info, &descriptor_set) != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate descriptor set.");
    }

    set_debug_name(
        device,
        VK_OBJECT_TYPE_DESCRIPTOR_SET,
        descriptor_set,
        debug_name.c_str()
    );
}

//  ----------------------------------------------------------------------------
void update_frame_descriptor_sets(
    VkDevice device,
    VkBuffer frame_uniform_buffer,
    size_t frame_ubo_size,
    VkDescriptorSet& descriptor_set
) {
    VkDescriptorBufferInfo frame_buffer_info{};
    frame_buffer_info.buffer = frame_uniform_buffer;
    frame_buffer_info.offset = 0;
    frame_buffer_info.range = frame_ubo_size;

    std::array<VkWriteDescriptorSet, 1> descriptor_writes{};

    //  Per-frame dynamic uniform buffer
    descriptor_writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptor_writes[0].dstSet = descriptor_set;
    descriptor_writes[0].dstBinding = 0;
    descriptor_writes[0].dstArrayElement = 0;
    descriptor_writes[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptor_writes[0].descriptorCount = 1;
    descriptor_writes[0].pBufferInfo = &frame_buffer_info;
    descriptor_writes[0].pImageInfo = nullptr;
    descriptor_writes[0].pTexelBufferView = nullptr;

    vkUpdateDescriptorSets(
        device,
        static_cast<uint32_t>(descriptor_writes.size()),
        descriptor_writes.data(),
        0,
        nullptr
    );
}


//  ----------------------------------------------------------------------------
void update_spine_descriptor_sets(
    VkDevice device,
    VkBuffer object_uniform_buffer,
    VkDescriptorSet& descriptor_set
) {
    VkDescriptorBufferInfo object_buffer_info{};
    object_buffer_info.buffer = object_uniform_buffer;
    object_buffer_info.offset = 0;
    object_buffer_info.range = VK_WHOLE_SIZE;

    std::array<VkWriteDescriptorSet, 1> descriptor_writes{};

    //  Per-object dynamic uniform buffer
    descriptor_writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptor_writes[0].dstSet = descriptor_set;
    descriptor_writes[0].dstBinding = 0;
    descriptor_writes[0].dstArrayElement = 0;
    descriptor_writes[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    descriptor_writes[0].descriptorCount = 1;
    descriptor_writes[0].pBufferInfo = &object_buffer_info;
    descriptor_writes[0].pImageInfo = nullptr;
    descriptor_writes[0].pTexelBufferView = nullptr;

    vkUpdateDescriptorSets(
        device,
        static_cast<uint32_t>(descriptor_writes.size()),
        descriptor_writes.data(),
        0,
        nullptr
    );
}

//  ----------------------------------------------------------------------------
void update_object_descriptor_sets(
    VkDevice device,
    const std::vector<Texture>& textures,
    VkBuffer object_uniform_buffer,
    VkDescriptorSet& descriptor_set
) {
    assert(!textures.empty());

    VkDescriptorBufferInfo object_buffer_info{};
    object_buffer_info.buffer = object_uniform_buffer;
    object_buffer_info.offset = 0;
    object_buffer_info.range = VK_WHOLE_SIZE;

    std::vector<VkDescriptorImageInfo> image_infos(textures.size());
    for (size_t n = 0; n < image_infos.size(); ++n) {
        image_infos[n].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        image_infos[n].imageView = textures[n].view;
        image_infos[n].sampler = textures[n].sampler;
    }

    std::array<VkWriteDescriptorSet, 2> descriptor_writes{};

    //  Per-object dynamic uniform buffer
    descriptor_writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptor_writes[0].dstSet = descriptor_set;
    descriptor_writes[0].dstBinding = 0;
    descriptor_writes[0].dstArrayElement = 0;
    descriptor_writes[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    descriptor_writes[0].descriptorCount = 1;
    descriptor_writes[0].pBufferInfo = &object_buffer_info;
    descriptor_writes[0].pImageInfo = nullptr;
    descriptor_writes[0].pTexelBufferView = nullptr;

    //  Combined texture sampler
    descriptor_writes[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptor_writes[1].dstSet = descriptor_set;
    descriptor_writes[1].dstBinding = 1;
    descriptor_writes[1].dstArrayElement = 0;
    descriptor_writes[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptor_writes[1].descriptorCount = static_cast<uint32_t>(image_infos.size());
    descriptor_writes[1].pBufferInfo = nullptr;
    descriptor_writes[1].pImageInfo = image_infos.data();
    descriptor_writes[1].pTexelBufferView = nullptr;

    vkUpdateDescriptorSets(
        device,
        static_cast<uint32_t>(descriptor_writes.size()),
        descriptor_writes.data(),
        0,
        nullptr
    );
}

//  ----------------------------------------------------------------------------
void create_descriptor_pool(
    VkDevice device,
    VkDescriptorPool& descriptor_pool
) {
    const uint32_t sampler_count = MAX_TEXTURES;
    const uint32_t max_sets = 2 + (sampler_count);

    std::array<VkDescriptorPoolSize, 3> pool_sizes{};
    pool_sizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    pool_sizes[0].descriptorCount = 1;
    pool_sizes[1].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    pool_sizes[1].descriptorCount = 1;
    pool_sizes[2].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    pool_sizes[2].descriptorCount = sampler_count;

    VkDescriptorPoolCreateInfo pool_info{};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.poolSizeCount = static_cast<uint32_t>(pool_sizes.size());
    pool_info.pPoolSizes = pool_sizes.data();
    pool_info.maxSets = max_sets;

    if (vkCreateDescriptorPool(device, &pool_info, nullptr, &descriptor_pool) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create descriptor pool.");
    }
}

//  ----------------------------------------------------------------------------
static void create_descriptor_objects(
    VkDevice device,
    DescriptorSetLayouts descriptor_set_layouts,
    const UniformBuffer<FrameUbo>& frame_uniform,
    const DynamicUniformBuffer<SpineUbo>& spine_uniform,
    const DynamicUniformBuffer<ObjectUbo>& object_uniform,
    const std::string& name_prefix,
    FrameDescriptorObjects& descriptor
) {
    create_descriptor_pool(device, descriptor.pool);

    create_descriptor_set(
        device,
        descriptor_set_layouts.frame,
        descriptor.pool,
        name_prefix + "_frame_descriptor_set",
        descriptor.frame_set
    );

    create_descriptor_set(
        device,
        descriptor_set_layouts.spine,
        descriptor.pool,
        name_prefix + "_spine_descriptor_set",
        descriptor.spine_set
    );

    create_descriptor_set(
        device,
        descriptor_set_layouts.texture_sampler,
        descriptor.pool,
        name_prefix + "_texture_sampler_descriptor_set",
        descriptor.texture_set
    );

    // create_descriptor_set(
    //     device,
    //     descriptor_set_layouts.object,
    //     descriptor.pool,
    //     name_prefix + "_object_descriptor_set",
    //     descriptor.object_set
    // );

    update_frame_descriptor_sets(
        device,
        frame_uniform.get_buffer(),
        frame_uniform.get_ubo_size(),
        descriptor.frame_set
    );

    update_spine_descriptor_sets(
        device,
        spine_uniform.get_buffer(),
        descriptor.spine_set
    );

    // update_object_descriptor_sets(
    //     device,
    //     textures,
    //     object_uniform.get_buffer(),
    //     descriptor.object_set
    // );
}

//  ----------------------------------------------------------------------------
static void create_secondary_command_objects(
    VkDevice device,
    VkPhysicalDevice physical_device,
    const std::string& name_prefix,
    ThreadFrameCommandObjects& frame_command
) {
    create_command_pool(
        device,
        physical_device,
        frame_command.pool,
        (name_prefix + "_command_pool").c_str()
    );

    // create_secondary_command_buffer(
    //     device,
    //     frame_command.pool,
    //     frame_command.buffer,
    //     (name_prefix + "_command_buffer").c_str()
    // );
}

//  ----------------------------------------------------------------------------
const char* RenderTaskManager::task_id_to_string(TaskId task_id) {
    switch (task_id) {
        default:
            return "?";
        case TaskId::DrawBillboards:
            return "draw_billboards";
        case TaskId::DrawModels:
            return "draw_models";
        case TaskId::DrawSprites:
            return "draw_sprites";
        case TaskId::UpdateFrameUniforms:
            return "update_frame_uniforms";
        case TaskId::UpdateObjectUniforms:
            return "update_object_uniforms";
    }
}

//  ----------------------------------------------------------------------------
bool RenderTaskManager::task_requires_textures(TaskId task_id) {
    switch (task_id) {
        default:
        case TaskId::DrawBillboards:
        case TaskId::DrawModels:
        case TaskId::DrawSprites:
            return true;

        case TaskId::UpdateFrameUniforms:
        case TaskId::UpdateObjectUniforms:
            return false;
    }
}

//  ----------------------------------------------------------------------------
RenderTaskManager::RenderTaskManager(
    VkPhysicalDevice physical_device,
    VkDevice device,
    DescriptorSetLayouts& descriptor_set_layouts,
    DescriptorSetManager& descriptor_set_mgr,
    ModelManager& model_mgr,
    TextureManager& texture_mgr,
    uint8_t frame_count,
    uint32_t max_objects
)
: m_frame_count(frame_count),
  m_max_objects(max_objects),
  m_physical_device(physical_device),
  m_device(device),
  m_descriptor_set_layouts(descriptor_set_layouts),
  m_descriptor_set_mgr(descriptor_set_mgr),
  m_model_mgr(model_mgr),
  m_texture_mgr(texture_mgr),
  m_uniform_buffers(frame_count)
{
    assert(m_frame_count > 0);

    //  Create uniform buffers for each frame
    for (auto& uniform_buffers : m_uniform_buffers) {
        uniform_buffers.frame.create(m_physical_device, m_device);
        uniform_buffers.object.create(m_physical_device, m_device, m_max_objects);
        uniform_buffers.spine.create(m_physical_device, m_device, m_max_objects);
    }
}

//  ----------------------------------------------------------------------------
RenderTaskManager::~RenderTaskManager() {
    cancel_threads();
}

//  ----------------------------------------------------------------------------
void RenderTaskManager::add_job(Job& job) {
    //  Can't enqueue jobs if frame was discarded.
    if (m_discard_frame) {
        return;
    }

    //  Check that a valid task ID was assigned
    if (job.task_id == TaskId::None) {
        throw std::runtime_error("Invalid job task ID.");
    }

    //  Track task call
    job.order = m_tasks.add_call(job.task_id);

    m_jobs.push(job);
}

//  ----------------------------------------------------------------------------
void RenderTaskManager::begin_frame(
    uint32_t cumulative_frame,
    uint8_t current_frame,
    bool discard_frame
) {
    m_cumulative_frame = cumulative_frame;
    m_current_frame = current_frame;
    m_discard_frame = discard_frame;
    m_tasks.clear();

    static bool first_call = true;
    if (first_call) {
        first_call = false;
        start_threads();
    }
}

//  ----------------------------------------------------------------------------
void RenderTaskManager::cancel_threads() {
    m_jobs.cancel();
    for (std::thread& thread : m_threads) {
        thread.join();
    }
    m_threads.clear();
    m_jobs.resume();
}

//  ----------------------------------------------------------------------------
bool RenderTaskManager::check_tasks_complete() {
    if (m_discard_frame) {
        return false;
    }

    if (m_tasks.is_complete()) {
        //  Worker threads have completed all tasks for this frame
        return true;
    }

    //  Worker threads are still processing tasks for this frame
    return false;
}

//  ----------------------------------------------------------------------------
void RenderTaskManager::draw_billboards(
    BillboardRenderer& renderer,
    const std::vector<SpriteBatch>& batches
) {
    if (batches.empty()) {
        // log_debug("Discarded draw billboards call with zero batches.");
        return;
    }

    Job job{};
    job.task_id = TaskId::DrawBillboards;
    job.renderer = &renderer;
    job.sprite_batches.reserve(batches.size());

    filter_pending_textures(batches, m_texture_mgr, job.sprite_batches);

    if (job.sprite_batches.empty()) {
        // log_debug("Discarded draw billboards call with zero batches.");
        return;
    }

    add_job(job);
}

//  ----------------------------------------------------------------------------
void RenderTaskManager::draw_glyphs(
    GlyphRenderer& renderer,
    const std::vector<GlyphBatch>& batches
) {
    if (batches.empty()) {
        // log_debug("Discarded draw glyphs call with zero batches.");
        return;
    }

    Job job{};
    job.task_id = TaskId::DrawGlyphs;
    job.renderer = &renderer;
    job.glyph_batches.reserve(batches.size());

    filter_pending_textures(batches, m_texture_mgr, job.glyph_batches);

    if (job.glyph_batches.empty()) {
        // log_debug("Discarded draw glyphs call with zero batches.");
        return;
    }

    add_job(job);
}

//  ----------------------------------------------------------------------------
void RenderTaskManager::draw_models(
    ModelRenderer& renderer,
    const std::vector<ModelBatch>& batches
) {
    if (batches.empty()) {
        // log_debug("Discarded draw models call with zero batches.");
        return;
    }

    Job job{};
    job.task_id = TaskId::DrawModels;
    job.renderer = &renderer;
    job.batches.reserve(batches.size());

    filter_pending_textures(batches, m_model_mgr, m_texture_mgr, job.batches);

    if (job.batches.empty()) {
        // log_debug("Discarded draw models call with zero batches.");
        return;
    }

    add_job(job);
}

//  ----------------------------------------------------------------------------
void RenderTaskManager::draw_spines(
    SpineSpriteRenderer& renderer,
    const std::vector<SpineSpriteBatch>& batches
) {
    //  Ignore empty batches. Batches with pending assets will have already been
    //  discarded.
    if (batches.empty()) {
        // log_debug("Discarded draw Spine sprites call with zero batches.");
        return;
    }

    Job job{};
    job.task_id = TaskId::DrawSpines;
    job.renderer = &renderer;
    job.spine_batches.reserve(batches.size());

    filter_pending_textures(batches, m_texture_mgr, job.spine_batches);

    if (job.spine_batches.empty()) {
        // log_debug("Discarded draw Spine sprites call with zero batches.");
        return;
    }

    add_job(job);
}

//  ----------------------------------------------------------------------------
void RenderTaskManager::draw_sprites(
    SpriteRenderer& renderer,
    const std::vector<SpriteBatch>& batches
) {
    if (batches.empty()) {
        // log_debug("Discarded draw sprites call with zero batches.");
        return;
    }

    Job job{};
    job.task_id = TaskId::DrawSprites;
    job.renderer = &renderer;
    job.sprite_batches.reserve(batches.size());

    filter_pending_textures(batches, m_texture_mgr, job.sprite_batches);

    if (job.sprite_batches.empty()) {
        // log_debug("Discarded draw sprites call with zero batches.");
        return;
    }

    add_job(job);
}

//  ----------------------------------------------------------------------------
void RenderTaskManager::end_frame() {
    m_discard_frame = false;
    m_tasks.clear();
}

//  ----------------------------------------------------------------------------
void RenderTaskManager::get_command_buffers(
    std::vector<VkCommandBuffer>& command_buffers
) {
    m_tasks.get_command_buffers(command_buffers);
}

//  ----------------------------------------------------------------------------
void RenderTaskManager::post_results(
    TaskId task_id,
    uint32_t order,
    VkCommandBuffer command_buffer
) {
    m_tasks.add_results(task_id, order, command_buffer);
}

//  ----------------------------------------------------------------------------
void RenderTaskManager::start_threads() {
    assert(m_thread_count > 0);
    for (auto n = 0; n < m_thread_count; ++n) {
        m_threads.emplace_back(&RenderTaskManager::thread_main, this, n);
    }
}

//  ----------------------------------------------------------------------------
void RenderTaskManager::thread_main(uint8_t thread_id) {
    Stopwatch stopwatch;

    log_debug("Render thread %d started (%p).", thread_id, std::this_thread::get_id());

    const std::string thread_name = "thread" + std::to_string(thread_id);

    //  Initialize frame command objects
    uint32_t frame_index = 0;
    std::vector<ThreadFrame> frames(m_frame_count);
    for (ThreadFrame& frame : frames) {
        frame.name = thread_name + "_frame" + std::to_string(frame_index);

        create_secondary_command_objects(
            m_device,
            m_physical_device,
            frame.name,
            frame.command
        );

        create_descriptor_objects(
            m_device,
            m_descriptor_set_layouts,
            m_uniform_buffers[frame_index].frame,
            m_uniform_buffers[frame_index].spine,
            m_uniform_buffers[frame_index].object,
            frame.name,
            frame.descriptor
        );

        ++frame_index;
    }

    bool frame_changed = false;
    uint32_t last_frame =  UINT32_MAX;

    //  Main loop
    while (true) {
        // log_debug(
        //     "%s: waiting (frame %d)",
        //     thread_name.c_str(),
        //     m_current_frame
        // );

        //  Wait for a job to process
        Job job{};
        if (!m_jobs.wait_and_pop(job)) {
            break;
        }

        //  Check if frame changed or if this thread is working multiple
        //  times this frame.
        //  Note that the frame this thread starts waiting for a job on is
        //  likely not the frame it will acquire and process a job on.
        if (last_frame != m_cumulative_frame) {
            last_frame = m_cumulative_frame;
            frame_changed = true;
        }

        // log_debug(
        //     "%s: acquired %s (frame: %d)",
        //     thread_name.c_str(),
        //     task_id_to_string(job.task_id),
        //     m_current_frame
        // );

        //  Get data for current frame
        ThreadFrame& frame = frames.at(m_current_frame);

        //  Check if frame has changed or if thread is continuing to process
        //  tasks during the same frame.
        if (frame_changed) {
            frame_changed = false;
            frame.command_buffer_index = 0;

            //  Reset command buffers
            vkResetCommandPool(
                m_device,
                frame.command.pool,
                VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT
            );
        } else {
            ++frame.command_buffer_index;
        }

        //  Create more command buffers if needed
        while (frame.command_buffer_index >= frame.command.buffers.size()) {
            VkCommandBuffer command_buffer;
            create_secondary_command_buffer(
                m_device,
                frame.command.pool,
                command_buffer,
                (frame.name+"_command_buffer"+std::to_string(frame.command.buffers.size())).c_str()
            );
            frame.command.buffers.push_back(command_buffer);
        }

        //  Update texture descriptors on the first task that requires them this frame.
        //  Textures are loaded on separate threads but new textures are not available
        //  until the start of a frame. Descriptors only need to be loaded to match
        //  textures changed since the start of the frame, as textures pending after
        //  the start of the frame will be removed from draw batches.
        if (task_requires_textures(job.task_id)) {
            //  After this task completes, the texture descriptors will be bound
            //  and cannot change again this frame.
            //  Texture timestamp should only change at the start of frames.
            const auto texture_timestamp = m_texture_mgr.get_timestamp();
            if (frame.texture_timestamp != texture_timestamp) {
                log_debug(
                    "%s: updating texture descriptor sets (frame: %d, timestamp: %d -> %d).",
                    thread_name.c_str(),
                    m_current_frame,
                    frame.texture_timestamp,
                    texture_timestamp
                );

                m_descriptor_set_mgr.copy_texture_descriptor_set(frame.descriptor.texture_set);

                frame.texture_timestamp = texture_timestamp;
            }
        }

        assert(frame.command_buffer_index < frame.command.buffers.size());

        //  Get command buffer to use
        VkCommandBuffer command_buffer = frame.command.buffers.at(frame.command_buffer_index);

        // log_debug(
        //     "%s:  execute %s",
        //     thread_name.c_str(),
        //     task_id_to_string(job.task_id)
        // );

        //  Process job
        switch (job.task_id) {
            case TaskId::DrawBillboards: {
                stopwatch.start(thread_name+"_draw_billboards");
                BillboardRenderer* billboard_renderer = static_cast<BillboardRenderer*>(job.renderer);
                billboard_renderer->draw_billboards(
                    job.sprite_batches,
                    frame.descriptor,
                    command_buffer
                );
                stopwatch.stop(thread_name+"_draw_billboards");
                break;
            }

            case TaskId::DrawGlyphs: {
                stopwatch.start(thread_name+"_draw_glyphs");
                GlyphRenderer* glyph_renderer = static_cast<GlyphRenderer*>(job.renderer);
                glyph_renderer->draw_glyphs(
                    job.glyph_batches,
                    frame.descriptor,
                    command_buffer
                );
                stopwatch.stop(thread_name+"_draw_glyphs");
                break;
            }

            case TaskId::DrawModels: {
                stopwatch.start(thread_name+"_draw_models");
                ModelRenderer* model_renderer = static_cast<ModelRenderer*>(job.renderer);
                model_renderer->draw_models(
                    job.batches,
                    frame.descriptor,
                    command_buffer
                );
                stopwatch.stop(thread_name+"_draw_models");
                break;
            }

            case TaskId::DrawSprites: {
                stopwatch.start(thread_name+"_draw_sprites");
                SpriteRenderer* sprite_renderer = static_cast<SpriteRenderer*>(job.renderer);
                sprite_renderer->draw_sprites(
                    job.sprite_batches,
                    frame.descriptor,
                    command_buffer
                );
                stopwatch.stop(thread_name+"_draw_sprites");
                break;
            }

            case TaskId::DrawSpines: {
                stopwatch.start(thread_name+"_draw_spines");
                SpineSpriteRenderer* spine_renderer = static_cast<SpineSpriteRenderer*>(job.renderer);
                auto& spine_uniform_buffer = m_uniform_buffers[m_current_frame].spine;
                spine_renderer->update_object_uniforms(job.spine_batches, spine_uniform_buffer);
                spine_renderer->draw_sprites(
                    job.spine_batches,
                    frame.descriptor,
                    spine_uniform_buffer,
                    command_buffer
                );
                stopwatch.stop(thread_name+"_draw_spines");
                break;
            }

            case TaskId::UpdateFrameUniforms: {
                stopwatch.start(thread_name+"_update_frame_uniforms");
                auto& frame_uniform_buffer = m_uniform_buffers[m_current_frame].frame;
                task_update_frame_uniforms(job.frame_ubo, frame_uniform_buffer);
                stopwatch.stop(thread_name+"_update_frame_uniforms");
                break;
            }

            case TaskId::UpdateObjectUniforms: {
                stopwatch.start(thread_name+"_update_object_uniforms");
                auto& object_uniform_buffer = m_uniform_buffers[m_current_frame].object;
                task_update_object_uniforms(job.batches, object_uniform_buffer);
                stopwatch.stop(thread_name+"_update_object_uniforms");
                break;
            }
        }

        // log_debug(
        //     "%s: finished %s",
        //     thread_name.c_str(),
        //     task_id_to_string(job.task_id)
        // );

        //  Post completed work
        post_results(job.task_id, job.order, command_buffer);
    }

    //  Release frame objects
    for (ThreadFrame& frame : frames) {
        //  Command pool
        vkDestroyCommandPool(m_device, frame.command.pool, nullptr);
        //  Descriptors
        vkDestroyDescriptorPool(m_device, frame.descriptor.pool, nullptr);
    }

    log_debug("Thread %d exited.", thread_id);
}

//  ----------------------------------------------------------------------------
void RenderTaskManager::shutdown() {
    //  Uniform buffers
    for (auto& uniform_buffer : m_uniform_buffers) {
        uniform_buffer.frame.destroy();
        uniform_buffer.object.destroy();
        uniform_buffer.spine.destroy();
    }
}

//  ----------------------------------------------------------------------------
void RenderTaskManager::update_frame_uniforms(
    const glm::mat4& view,
    const glm::mat4& proj,
    const glm::mat4& ortho_view,
    const glm::mat4& ortho_proj
) {
    //  Update uniform data
    Job job{};
    job.task_id = TaskId::UpdateFrameUniforms;
    job.frame_ubo.view = view;
    job.frame_ubo.proj = proj;
    job.frame_ubo.ortho_view = ortho_view;
    job.frame_ubo.ortho_proj = ortho_proj;
    add_job(job);
}
}
