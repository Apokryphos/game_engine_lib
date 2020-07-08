#pragma once

#include "engine/screens/screen.hpp"
#include <glm/vec3.hpp>
#include <vector>

namespace demo
{
class DemoScreen : public engine::Screen
{
    using Game = engine::Game;

    std::vector<glm::vec3> m_positions;

protected:
    virtual void on_activate(Game& game) override;
    virtual void on_load(Game& game) override;
    virtual void on_render(Game& game) override;
    virtual void on_update(Game& game) override;

public:
    DemoScreen()
    : Screen("demo_screen") {
    }
};
}
