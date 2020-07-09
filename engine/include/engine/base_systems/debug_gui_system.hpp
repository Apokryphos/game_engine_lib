#pragma once

#include "common/system.hpp"
#include "engine/base_systems/base_system_ids.hpp"
#include "engine/debug_gui/entity_debug_info.hpp"
#include <vector>

namespace engine
{
class DebugGui;
class Game;

//  Debug GUI (ImGui)
class DebugGuiSystem : public common::System
{
public:
    typedef std::function<void (Game&)> DebugGuiFunc;

private:
    struct Entry
    {
        bool visible;
        //  Name that appears in system manager window
        std::string title;
        DebugGui* debug_gui;
        DebugGuiFunc func;
    };

    bool m_visible;
    std::vector<Entry> m_entries;
    std::vector<EntityDebugInfo> m_entity_debug_infos;
    std::vector<std::unique_ptr<DebugGui>> m_debug_gui;

    void update_system_manager_gui(Game& game);

public:
    DebugGuiSystem();
    //  Adds a DebugGui object
    void add_gui(std::unique_ptr<DebugGui> debug_gui);
    //  Adds a debug GUI function
    void add_gui(const std::string& title, DebugGuiFunc func);
    static const common::SystemId Id = SYSTEM_ID_DEBUG_GUI;
    const std::vector<EntityDebugInfo>& get_entity_debug_infos() const;
    void toggle_visible();
    void update(Game& game);
};
}
