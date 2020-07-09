#include "common/system.hpp"
#include "engine/base_systems/base_system_util.hpp"
#include "engine/base_systems/config_system.hpp"
#include "engine/base_systems/debug_gui_system.hpp"
#include "engine/base_systems/debug_gui_util.hpp"
#include "engine/base_systems/profile_system.hpp"
#include "engine/debug_gui/config_system_debug_gui.hpp"
#include "engine/debug_gui/profile_system_debug_gui.hpp"
#include "engine/game.hpp"
#include "engine/system_manager.hpp"

#include "imgui.h"

using namespace common;

namespace engine
{
//  ----------------------------------------------------------------------------
ConfigSystem& get_config_system(SystemManager& sys_mgr) {
    return sys_mgr.get_system<ConfigSystem>(SYSTEM_ID_CONFIG);
}

//  ----------------------------------------------------------------------------
DebugGuiSystem& get_debug_gui_system(SystemManager& sys_mgr) {
    return sys_mgr.get_system<DebugGuiSystem>(SYSTEM_ID_DEBUG_GUI);
}

//  ----------------------------------------------------------------------------
ProfileSystem& get_profile_system(SystemManager& sys_mgr) {
    return sys_mgr.get_system<ProfileSystem>(SYSTEM_ID_PROFILE);
}

//  ----------------------------------------------------------------------------
void initialize_base_systems(
    Game& game,
    const std::string& game_base_name
) {
    SystemManager& sys_mgr = game.get_system_manager();

    //  Debug GUI system
    sys_mgr.add_system(std::make_unique<DebugGuiSystem>());

    //  Configuration system
    sys_mgr.add_system(std::make_unique<ConfigSystem>(game_base_name));
    add_debug_gui<ConfigSystem>(
        game,
        get_config_system(sys_mgr),
        config_system_debug_gui
    );

    //  Profile system
    sys_mgr.add_system(std::make_unique<ProfileSystem>(game_base_name));
    add_debug_gui<ProfileSystem>(
        game,
        get_profile_system(sys_mgr),
        profile_system_debug_gui
    );
}
}
