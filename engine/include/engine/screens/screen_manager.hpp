#pragma once

#include <memory>
#include <string>
#include <vector>

namespace engine
{
class Game;
class Screen;

class ScreenManager
{
    Screen* m_active;
    std::vector<std::unique_ptr<Screen>> m_screens;

    Screen* find_screen(const std::string& screen_name);

public:
    ScreenManager();
    ~ScreenManager();
    ScreenManager(const ScreenManager&) = delete;
    ScreenManager& operator=(const ScreenManager&) = delete;
    void activate(Game& game, const std::string& screen_name);
    void add_screen(Game& game, std::unique_ptr<Screen> screen);
    void render(Game& game);
    void update(Game& game);
};
}
