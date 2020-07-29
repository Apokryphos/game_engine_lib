#include "common/log.hpp"
#include "common/stopwatch.hpp"
#include "render_vk/command_buffer.hpp"
#include "render_vk/command_pool.hpp"
#include "render_vk/debug_utils.hpp"
#include "render_vk/descriptor_pool.hpp"
#include "render_vk/descriptor_set_layout.hpp"
#include "render_vk/model_manager.hpp"
#include "render_vk/render_task_manager.hpp"
#include "render_vk/render_tasks/task_update_uniforms.hpp"
#include "render_vk/renderers/billboard_renderer.hpp"
#include "render_vk/renderers/model_renderer.hpp"
#include "render_vk/renderers/sprite_renderer.hpp"
#include "render_vk/texture.hpp"
#include "render_vk/texture_manager.hpp"
#include "render_vk/vulkan_queue.hpp"
#include "render_vk/vulkan_swapchain.hpp"

using namespace common;
using namespace render;

namespace render_vk
{
static Stopwatch STOPWATCH;

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
void update_texture_descriptor_sets(
    VkDevice device,
    const std::vector<Texture>& textures,
    VkDescriptorSet& descriptor_set
) {
    std::vector<VkDescriptorImageInfo> image_infos(textures.size());
    for (size_t n = 0; n < image_infos.size(); ++n) {
        image_infos[n].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        image_infos[n].imageView = textures[n].view;
        image_infos[n].sampler = textures[n].sampler;
    }

    std::array<VkWriteDescriptorSet, 1> descriptor_writes{};

    //  Combined texture sampler
    descriptor_writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptor_writes[0].dstSet = descriptor_set;
    descriptor_writes[0].dstBinding = 0;
    descriptor_writes[0].dstArrayElement = 0;
    descriptor_writes[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptor_writes[0].descriptorCount = static_cast<uint32_t>(image_infos.size());
    descriptor_writes[0].pBufferInfo = nullptr;
    descriptor_writes[0].pImageInfo = image_infos.data();
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

    std::array<VkDescriptorPoolSize, 2> pool_sizes{};
    pool_sizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    pool_sizes[0].descriptorCount = 1;
    // pool_sizes[1].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    // pool_sizes[1].descriptorCount = 1;
    pool_sizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    pool_sizes[1].descriptorCount = sampler_count;

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
    const std::vector<Texture>& textures,
    const UniformBuffer<FrameUbo>& frame_uniform,
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

    update_texture_descriptor_sets(
        device,
        textures,
        descriptor.texture_set
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
    FrameCommandObjects& frame_command
) {
    create_command_pool(
        device,
        physical_device,
        frame_command.pool,
        (name_prefix + "_command_pool").c_str()
    );

    create_secondary_command_buffer(
        device,
        frame_command.pool,
        frame_command.buffer,
        (name_prefix + "_command_buffer").c_str()
    );
}

//  ----------------------------------------------------------------------------
RenderTaskManager::RenderTaskManager(
    VkPhysicalDevice physical_device,
    VkDevice device,
    DescriptorSetLayouts& descriptor_set_layouts,
    UniformBuffer<FrameUbo>& frame_uniform,
    DynamicUniformBuffer<ObjectUbo>& object_uniform,
    ModelManager& model_mgr,
    TextureManager& texture_mgr
)
: m_physical_device(physical_device),
  m_device(device),
  m_descriptor_set_layouts(descriptor_set_layouts),
  m_frame_uniform(frame_uniform),
  m_object_uniform(object_uniform),
  m_model_mgr(model_mgr),
  m_texture_mgr(texture_mgr) {
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
    {
        std::lock_guard<std::mutex> lock(m_tasks_mutex);
        job.order = m_tasks.add_call(job.task_id);
    }

    //  Enqueue job
    {
        std::lock_guard<std::mutex> lock(m_jobs_mutex);
        m_jobs.push(job);
    }
}

//  ----------------------------------------------------------------------------
void RenderTaskManager::begin_frame(
    uint32_t current_frame,
    bool discard_frame
) {
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
    m_cancel_threads = true;
    for (std::thread& thread : m_threads) {
        thread.join();
    }
    m_threads.clear();
    m_cancel_threads = false;
}

//  ----------------------------------------------------------------------------
bool RenderTaskManager::check_tasks_complete() {
    if (m_discard_frame) {
        return false;
    }

    {
        std::lock_guard<std::mutex> lock(m_tasks_mutex);

        if (m_tasks.is_complete()) {
            //  Worker threads have completed all tasks for this frame
            return true;
        }
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
        return;
    }

    Job job{};
    job.task_id = TaskId::DrawBillboards;
    job.renderer = &renderer;
    job.sprite_batches = batches;
    add_job(job);
}


//  ----------------------------------------------------------------------------
void RenderTaskManager::draw_models(
    ModelRenderer& renderer,
    const std::vector<ModelBatch>& batches
) {
    if (batches.empty()) {
        return;
    }

    Job job{};
    job.task_id = TaskId::DrawModels;
    job.batches = batches;
    job.renderer = &renderer;
    add_job(job);
}

//  ----------------------------------------------------------------------------
void RenderTaskManager::draw_sprites(
    SpriteRenderer& renderer,
    const std::vector<SpriteBatch>& batches
) {
    if (batches.empty()) {
        return;
    }

    Job job{};
    job.task_id = TaskId::DrawSprites;
    job.renderer = &renderer;
    job.sprite_batches = batches;
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
bool RenderTaskManager::get_job(Job& job) {
    std::lock_guard<std::mutex> lock(m_jobs_mutex);

    if (m_jobs.empty()) {
        return false;
    }

    job = m_jobs.front();
    m_jobs.pop();

    return true;
}

//  ----------------------------------------------------------------------------
void RenderTaskManager::post_results(
    TaskId task_id,
    uint32_t order,
    VkCommandBuffer command_buffer
) {
    std::lock_guard<std::mutex> lock(m_tasks_mutex);
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
    log_debug("Thread %d started.", thread_id);

    const std::string thread_name = "thread" + std::to_string(thread_id);

    //  Initialize frame command objects
    uint32_t frame_index = 0;
    std::vector<ThreadFrame> frames(m_frame_count);
    for (ThreadFrame& frame : frames) {
        frame.name = thread_name + "_frame" + std::to_string(++frame_index);

        create_secondary_command_objects(
            m_device,
            m_physical_device,
            frame.name,
            frame.command
        );
    }

    //  Main loop
    while (!m_cancel_threads) {
        Job job{};
        //  Sleep until job is available
        if (!get_job(job)) {
            std::this_thread::sleep_for(std::chrono::microseconds(250));
            continue;
        }

        //  Get data for current frame
        ThreadFrame& frame = frames.at(m_current_frame);

        //  Check if textures changed since last frame
        if (frame.texture_timestamp != m_texture_mgr.get_timestamp()) {
            std::vector<Texture> textures;
            m_texture_mgr.get_textures(textures);

            //  Recreate descriptor objects
            create_descriptor_objects(
                m_device,
                m_descriptor_set_layouts,
                textures,
                m_frame_uniform,
                m_object_uniform,
                frame.name,
                frame.descriptor
            );

            frame.texture_timestamp = m_texture_mgr.get_timestamp();

            log_debug(
                "Render worker thread '%s' updated descriptor sets after texture changes.",
                frame.name.c_str()
            );
        }

        vkResetCommandPool(
            m_device,
            frame.command.pool,
            VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT
        );

        //  Process job
        switch (job.task_id) {
            case TaskId::DrawBillboards: {
                STOPWATCH.start(thread_name+"_draw_billboards");
                BillboardRenderer* billboard_renderer = static_cast<BillboardRenderer*>(job.renderer);
                billboard_renderer->draw_billboards(
                    job.sprite_batches,
                    frame.descriptor,
                    frame.command.buffer
                );
                STOPWATCH.stop(thread_name+"_draw_billboards");
                break;
            }

            case TaskId::DrawModels: {
                STOPWATCH.start(thread_name+"_draw_models");
                ModelRenderer* model_renderer = static_cast<ModelRenderer*>(job.renderer);
                model_renderer->draw_models(
                    job.batches,
                    frame.descriptor,
                    frame.command.buffer
                );
                STOPWATCH.stop(thread_name+"_draw_models");
                break;
            }

            case TaskId::DrawSprites: {
                STOPWATCH.start(thread_name+"_draw_sprites");
                SpriteRenderer* sprite_renderer = static_cast<SpriteRenderer*>(job.renderer);
                sprite_renderer->draw_sprites(
                    job.sprite_batches,
                    frame.descriptor,
                    frame.command.buffer
                );
                STOPWATCH.stop(thread_name+"_draw_sprites");
                break;
            }

            case TaskId::UpdateFrameUniforms:
                STOPWATCH.start(thread_name+"_update_frame_uniforms");
                task_update_frame_uniforms(job.frame_ubo, m_frame_uniform);
                STOPWATCH.stop(thread_name+"_update_frame_uniforms");
                break;

            case TaskId::UpdateObjectUniforms:
                STOPWATCH.start(thread_name+"_update_object_uniforms");
                task_update_object_uniforms(job.batches, m_object_uniform);
                STOPWATCH.stop(thread_name+"_update_object_uniforms");
                break;
        }

        //  Post completed work
        post_results(job.task_id, job.order, frame.command.buffer);
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
