#pragma once

#include "engine/screens/screen.hpp"

namespace demo
{
//  Initialization screen.
//  Initializes game and loads initial assets.
//  Not seen by user.
class InitScreen : public engine::Screen
{
    using Game = engine::Game;

protected:
    virtual void on_load(Game& game) override;

public:
    InitScreen()
    : Screen("init_screen") {
    }
};
}
