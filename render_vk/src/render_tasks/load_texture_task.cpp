#include "common/log.hpp"
#include "common/thread_manager.hpp"
#include "render_vk/model_manager.hpp"
#include "render_vk/render_tasks/load_texture_task.hpp"
#include "render_vk/texture.hpp"
#include <string>

using namespace common;

namespace render_vk
{
//  ----------------------------------------------------------------------------
RenderJobResult task_load_texture(RenderThreadState& state, LoadTextureArgs& args) {
    Texture texture{};

    create_texture(
        state.physical_device,
        state.device,
        *state.graphics_queue,
        state.command_pool,
        args.path,
        texture
    );

    state.model_mgr->add_texture(args.id, texture);

    RenderJobResult result{};
    return result;
}
}
