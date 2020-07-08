#pragma once

#include "engine/ui/ui_state.hpp"

namespace demo
{
class Game;
};

namespace input
{
class InputEvent;
};

namespace demo
{
class DemoState : public engine::UiState
{
    using Game = engine::Game;
    using InputEvent = input::InputEvent;

protected:

    virtual void on_process_event(Game& game, const InputEvent& event) override;

public:
    DemoState()
    : UiState("demo_state") {
    }
};
}
