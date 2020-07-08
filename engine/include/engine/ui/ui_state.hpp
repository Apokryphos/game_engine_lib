#pragma once

#include <string>

namespace input
{
class InputEvent;
};

namespace engine
{
class Game;

class UiState
{
    using InputEvent = input::InputEvent;

    friend class UiStateManager;

    enum class State
    {
        None,
        Loaded,
        Activated,
    };

    State m_state;
    std::string m_name;

protected:
    void activate(Game& game);
    void deactivate(Game& game);
    void load(Game& game);
    void process_event(Game& game, const InputEvent& event);

    virtual void on_activate(Game& game) {}
    virtual void on_deactivate(Game& game) {}
    virtual void on_load(Game& game) {}
    virtual void on_process_event(Game& game, const InputEvent& event) {}

public:
    UiState(const std::string& name)
    : m_state(State::None),
      m_name(name) {
    }

    virtual ~UiState() {}
    UiState(const UiState&) = delete;
    UiState& operator=(const UiState&) = delete;

    const std::string& get_name() const {
        return m_name;
    }
};
}
