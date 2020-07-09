#pragma once

#include "engine/base_systems/profile_system.hpp"
#include "engine/debug_gui/debug_gui.hpp"

namespace engine
{
class Game;
class ProfileSystem;

class ProfileSystemDebugGui : public SystemDebugGui<ProfileSystem>
{
protected:
    virtual void on_update(Game& game) override;

public:
    ProfileSystemDebugGui(ProfileSystem& config_sys);
};
}
