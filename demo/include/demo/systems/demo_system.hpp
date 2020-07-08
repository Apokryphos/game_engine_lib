#pragma once

#include "common/system.hpp"
#include "systems/system_ids.hpp"
#include <glm/vec3.hpp>

namespace engine
{
class Game;
};

namespace demo
{
const common::SystemId SYSTEM_ID_DEMO = systems::SYSTEM_ID_LAST + 1;

class DemoSystem : public common::System
{
    float m_fov;
    float m_rotate;
    glm::vec3 m_position;

public:
    DemoSystem();
    void forward(float amount);
    float get_fov() const;
    glm::vec3 get_position() const;
    float get_rotate() const;
    void rotate(float amount);
    void strafe(float amount);
    void update(engine::Game& game);
    void zoom(float amount);
};
}
