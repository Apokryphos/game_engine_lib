#pragma once

#include <memory>
#include <string>
#include <vector>

namespace input
{
class InputEvent;
};

namespace engine
{
class Game;
class UiState;

class UiStateManager
{
    using InputEvent = input::InputEvent;

    UiState* m_active;
    std::vector<std::unique_ptr<UiState>> m_states;

    UiState* find_state(const std::string& state_name);

public:
    UiStateManager();
    ~UiStateManager();
    UiStateManager(const UiStateManager&) = delete;
    UiStateManager& operator=(const UiStateManager&) = delete;
    void activate(Game& game, const std::string& state_name);
    void add_state(Game& game, std::unique_ptr<UiState> state);
    void process_event(Game& game, const InputEvent& event);
};
}
