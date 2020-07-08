#pragma once

#include <string>

namespace engine
{
class Game;

class Screen
{
    friend class ScreenManager;

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
    void render(Game& game);
    void update(Game& game);

    virtual void on_activate(Game& game) {}
    virtual void on_deactivate(Game& game) {}
    virtual void on_load(Game& game) {}
    virtual void on_render(Game& game) {}
    virtual void on_update(Game& game) {}

public:
    Screen(const std::string& name)
    : m_state(State::None),
      m_name(name) {
    }

    virtual ~Screen() {}
    Screen(const Screen&) = delete;
    Screen& operator=(const Screen&) = delete;

    const std::string& get_name() const {
        return m_name;
    }
};
}
