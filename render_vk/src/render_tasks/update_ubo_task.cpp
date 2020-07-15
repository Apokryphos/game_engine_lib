#include "common/log.hpp"
#include "common/thread_manager.hpp"
#include "render_vk/debug_utils.hpp"
#include "render_vk/mesh.hpp"
#include "render_vk/model_manager.hpp"
#include "render_vk/render_tasks/update_ubo_task.hpp"
#include "render_vk/ubo.hpp"
#include "render_vk/vulkan.hpp"
#include "render_vk/vulkan_model.hpp"
#include "render_vk/vulkan_renderer.hpp"
#include <string>
#include <glm/gtc/matrix_transform.hpp>

using namespace common;

namespace render_vk
{
//  ----------------------------------------------------------------------------
void task_update_ubo(RenderThreadState& state, UpdateUboArgs& args) {
    const size_t count = args.positions.size();

    //  Update all UBO structs once per frame
    std::vector<ObjectUbo> data(count);
    for (size_t n = 0; n < count; ++n)  {
        data[n].texture_index = args.texture_ids[n];
        data[n].model = glm::translate(glm::mat4(1.0f), args.positions[n]);
    }

    //  Copy object UBO structs to dynamic uniform buffer
    args.object_uniform->copy(data);

    state.renderer->post_frame(args.frame, RENDER_TASK_UPDATE_UBO, VK_NULL_HANDLE, state.descriptor_sets[args.swapchain_image]);
}
}
