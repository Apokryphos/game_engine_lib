#include "common/system.hpp"
#include "engine/base_systems/base_system_util.hpp"
#include "engine/base_systems/config_system.hpp"
#include "engine/base_systems/debug_gui_system.hpp"
#include "engine/base_systems/editor_system.hpp"
#include "engine/base_systems/name_system.hpp"
#include "engine/base_systems/profile_system.hpp"
#include "engine/debug_gui/config_system_debug_panel.hpp"
#include "engine/debug_gui/debug_gui_system_debug_panel.hpp"
#include "engine/debug_gui/name_system_debug_panel.hpp"
#include "engine/debug_gui/profile_system_debug_panel.hpp"
#include "engine/editor/editor_panel.hpp"
#include "engine/editor/name_system_editor_panel.hpp"
#include "engine/game.hpp"
#include "engine/system_manager.hpp"

using namespace common;
using namespace ecs;

namespace engine
{
//  ----------------------------------------------------------------------------
void add_name_component(
    const Entity entity,
    NameSystem& name_sys,
    const std::string& name
) {
    if (!name_sys.has_component(entity)) {
        name_sys.add_component(entity);
    }

    const auto name_cmpnt = name_sys.get_component(entity);
    name_sys.set_name(name_cmpnt, name);
}

//  ----------------------------------------------------------------------------
ConfigSystem& get_config_system(SystemManager& sys_mgr) {
    return sys_mgr.get_system<ConfigSystem>(SYSTEM_ID_CONFIG);
}

//  ----------------------------------------------------------------------------
DebugGuiSystem& get_debug_gui_system(SystemManager& sys_mgr) {
    return sys_mgr.get_system<DebugGuiSystem>(SYSTEM_ID_DEBUG_GUI);
}

//  ----------------------------------------------------------------------------
EditorSystem& get_editor_system(SystemManager& sys_mgr) {
    return sys_mgr.get_system<EditorSystem>(SYSTEM_ID_EDITOR);
}

//  ----------------------------------------------------------------------------
NameSystem& get_name_system(SystemManager& sys_mgr) {
    return sys_mgr.get_system<NameSystem>(SYSTEM_ID_NAME);
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
    DebugGuiSystem& debug_gui_system = get_debug_gui_system(sys_mgr);
    debug_gui_system.add_gui(
        std::make_unique<DebugGuiSystemDebugPanel>(get_debug_gui_system(sys_mgr)
    ));

    //  Editor GUI system
    sys_mgr.add_system(std::make_unique<EditorSystem>());

    //  Configuration system
    sys_mgr.add_system(std::make_unique<ConfigSystem>(game_base_name));
    debug_gui_system.add_gui(
        std::make_unique<ConfigSystemDebugPanel>(get_config_system(sys_mgr))
    );

    //  Profile system
    sys_mgr.add_system(std::make_unique<ProfileSystem>(game_base_name));
    debug_gui_system.add_gui(
        std::make_unique<ProfileSystemDebugPanel>(get_profile_system(sys_mgr))
    );

    EcsRoot& ecs = game.get_ecs_root();

    //  Name system
    sys_mgr.add_system(std::make_unique<NameSystem>(ecs, 1000));
    debug_gui_system.add_gui(
        std::make_unique<NameSystemDebugPanel>(get_name_system(sys_mgr))
    );

    //  Editors
    EditorSystem& editor_sys = get_editor_system(sys_mgr);
    editor_sys.add_panel(
        std::make_unique<NameSystemEditorPanel>(get_name_system(sys_mgr))
    );
}
}
