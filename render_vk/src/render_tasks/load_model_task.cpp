#include "common/log.hpp"
#include "common/thread_manager.hpp"
#include "render_vk/mesh.hpp"
#include "render_vk/model_manager.hpp"
#include "render_vk/render_tasks/load_model_task.hpp"
#include "render_vk/vulkan_model.hpp"
#include <string>

using namespace common;

namespace render_vk
{
//  ----------------------------------------------------------------------------
void task_load_model(RenderThreadState& state, LoadModelArgs& args) {
    Mesh mesh;
    load_mesh(mesh, args.path);

    auto model = std::make_unique<VulkanModel>(args.id, args.path);
    model->load(
        state.physical_device,
        state.device,
        state.graphics_queue->get_queue(),
        state.command_pool,
        mesh
    );

    state.model_mgr->add_model(std::move(model));
}
}
