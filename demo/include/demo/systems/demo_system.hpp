#pragma once

#include "common/system.hpp"
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
public:
    struct DrawOrder
    {
        std::vector<uint32_t> model_ids;
        std::vector<glm::vec3> positions;
        std::vector<uint32_t> texture_ids;
    };

private:

public:
    DemoSystem();
    void build_draw_order(
        engine::Game& game,
        DrawOrder& draw_order
    );
};
}
