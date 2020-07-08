#pragma once

#include "engine/screens/screen.hpp"

namespace demo
{
class DemoScreen : public engine::Screen
{
    using Game = engine::Game;

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
