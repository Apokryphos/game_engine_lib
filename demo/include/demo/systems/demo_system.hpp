#pragma once

#include "common/system.hpp"
#include "systems/system_ids.hpp"
#include <glm/mat4x4.hpp>

namespace engine
{
class Game;
};

namespace demo
{
const common::SystemId SYSTEM_ID_DEMO = systems::SYSTEM_ID_LAST + 1;

class DemoSystem : public common::System
{
public:
    struct DrawOrder
    {
        glm::mat4x4 model;
        uint32_t model_id;
    };

private:

public:
    DemoSystem();
    void build_draw_order(
        engine::Game& game,
        std::vector<DrawOrder>& draw_order
    );
};
}
