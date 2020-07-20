#pragma once

#include "common/system.hpp"
#include "render/renderers/model_renderer.hpp"
#include "systems/system_ids.hpp"
#include <glm/mat4x4.hpp>
#include <vector>

namespace engine
{
class Game;
};

namespace demo
{
const common::SystemId SYSTEM_ID_DEMO = systems::SYSTEM_ID_LAST + 1;

class DemoSystem : public common::System
{
private:

public:
    DemoSystem();
    void batch_models(
        engine::Game& game,
        glm::mat4 view,
        glm::mat4 proj,
        std::vector<render::ModelBatch>& batches
    );
};
}
