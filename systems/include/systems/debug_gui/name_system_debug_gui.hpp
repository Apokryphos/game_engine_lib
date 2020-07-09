#pragma once

namespace engine
{
class Game;
}

namespace systems
{
class NameSystem;

void name_system_debug_gui(engine::Game& game, NameSystem& name_sys);
}
