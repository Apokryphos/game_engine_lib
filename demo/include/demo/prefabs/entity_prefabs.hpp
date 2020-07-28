#pragma once

#include "ecs/entity.hpp"
#include <glm/vec3.hpp>
#include <string>

namespace engine
{
class Game;
}

namespace demo
{
void make_camera(
    engine::Game& game,
    const ecs::Entity entity,
    const std::string& name,
    glm::vec3 position,
    float move_speed,
    bool ortho = false
);
}
