#pragma once

#include "common/system.hpp"
#include "systems/system_ids.hpp"

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
    DemoSystem();
    void update(engine::Game& game);
};
}
