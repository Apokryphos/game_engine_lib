#pragma once

#include "common/system.hpp"
#include "ecs/entity_handle.hpp"
#include "engine/base_systems/base_system_ids.hpp"
#include "engine/debug_gui/entity_info.hpp"
#include <vector>

namespace engine
{
class EditorPanel;
class Game;

//  Editor GUI (ImGui)
class EditorSystem : public common::System
{
public:
    typedef std::function<void (Game&, const ecs::Entity)> EditorGuiFunc;

private:
    struct Entry
    {
        bool visible;
        //  Name that appears in editor panel
        std::string title;
        EditorPanel* panel;
        EditorGuiFunc func;
    };

    bool m_visible;
    ecs::EntityHandle m_entity;
    std::vector<EntityInfo> m_entity_infos;
    std::vector<Entry> m_entries;
    std::vector<std::unique_ptr<EditorPanel>> m_panels;

    void update_system_manager_gui(Game& game);

public:
    EditorSystem();
    //  Adds an EditorPanel object
    void add_panel(std::unique_ptr<EditorPanel> panel);
    //  Adds an editor GUI function
    void add_panel(const std::string& title, EditorGuiFunc func);
    static const common::SystemId Id = SYSTEM_ID_EDITOR;

    ecs::EntityHandle& get_selected_entity() {
        return m_entity;
    }

    void toggle_visible();
    void update(Game& game);
};
}
