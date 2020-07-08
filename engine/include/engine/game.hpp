#pragma once

#include "common/random.hpp"
#include <memory>
#include <string>

namespace ecs
{
class EcsRoot;
}

namespace platform
{
struct WindowOptions;
}

namespace engine
{
class Engine;
class SystemManager;

class Game
{
    using EcsRoot = ecs::EcsRoot;
    using Random = common::Random;
    using WindowOptions = platform::WindowOptions;

    bool m_quit;
    Random m_random;
    std::unique_ptr<EcsRoot> m_ecs_root;
    std::unique_ptr<Engine> m_engine;
    std::unique_ptr<SystemManager> m_sys_mgr;

    void render();
    void update();

public:
    Game();
    ~Game();
    Game(const Game&) = delete;
    Game& operator=(const Game&) = delete;
    EcsRoot& get_ecs_root();
    Engine& get_engine();

    Random& get_random() {
        return m_random;
    }

    SystemManager& get_system_manager();

    bool initialize(
        const std::string& title,
        const WindowOptions& window_options
    );

    void quit();
    void run();
    void shutdown();
};
}
