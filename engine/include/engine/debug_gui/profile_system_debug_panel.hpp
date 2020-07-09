#pragma once

#include "engine/base_systems/profile_system.hpp"
#include "engine/debug_gui/system_debug_panel.hpp"

namespace engine
{
class Game;
class ProfileSystem;

class ProfileSystemDebugPanel : public SystemDebugPanel<ProfileSystem>
{
protected:
    virtual void on_update(Game& game) override;

public:
    ProfileSystemDebugPanel(ProfileSystem& config_sys);
};
}
