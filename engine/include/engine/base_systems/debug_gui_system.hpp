#pragma once

#include "common/system.hpp"
#include "engine/base_systems/base_system_ids.hpp"
#include <map>

namespace engine
{
class Game;

//  Debug GUI (ImGui)
class DebugGuiSystem : public common::System
{
    using SystemId = common::SystemId;

public:
    typedef std::function<void (Game&)> DebugGuiFunc;

private:
    struct Entry
    {
        bool visible;
        DebugGuiFunc func;
    };

    bool m_visible;
    std::map<SystemId, Entry> m_entries;

    void update_system_manager_gui(Game& game);

public:
    DebugGuiSystem();
    void add_gui(const common::SystemId system_id, DebugGuiFunc func);
    static const common::SystemId Id = SYSTEM_ID_DEBUG_GUI;
    void toggle_visible();
    void update(Game& game);
};
}
