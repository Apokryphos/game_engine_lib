#pragma once

#include "common/thread_manager.hpp"

namespace render_vk
{
const common::TaskId RENDER_TASK_DRAW_MODELS  = 1;
const common::TaskId RENDER_TASK_LOAD_MODEL   = 2;
const common::TaskId RENDER_TASK_LOAD_TEXTURE = 3;
}
