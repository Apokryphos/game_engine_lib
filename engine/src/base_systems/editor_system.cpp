#include "ecs/ecs_root.hpp"
#include "engine/base_systems/editor_system.hpp"
#include "engine/editor/editor_panel.hpp"
#include "engine/game.hpp"
#include "imgui.h"

using namespace ecs;

namespace engine
{
//  ----------------------------------------------------------------------------
EditorSystem::EditorSystem()
: System(SYSTEM_ID_EDITOR, "editor_system") {
}

//  ----------------------------------------------------------------------------
void EditorSystem::add_panel(std::unique_ptr<EditorPanel> panel) {
    if (panel == nullptr) {
        throw std::runtime_error("Editor panel cannot be null.");
    }

    Entry entry{};
    entry.title = panel->get_panel_title();
    entry.panel = panel.get();
    entry.visible = true;

    m_entries.push_back(entry);

    m_panels.push_back(std::move(panel));
}

//  ----------------------------------------------------------------------------
void EditorSystem::add_panel(const std::string& title, EditorGuiFunc func) {
    if (!func) {
        throw std::runtime_error("Editor panel function cannot be null.");
    }

    Entry entry{};
    entry.title = title;
    entry.func = func;
    entry.visible = true;

    m_entries.push_back(entry);
}

//  ----------------------------------------------------------------------------
void EditorSystem::toggle_visible() {
    m_visible = !m_visible;
}

//  ----------------------------------------------------------------------------
void EditorSystem::update(Game& game) {
    if (!m_visible) {
        return;
    }

    if (!ImGui::Begin("editor")) {
        ImGui::End();
        return;
    }

    update_system_manager_gui(game);

    if (!m_entity.has_entity()) {
        ImGui::Text("No entity selected.");
        ImGui::End();
        return;
    }

    const Entity entity = m_entity.get_entity();

    //  Update debug panels
    for (const Entry& entry : m_entries) {
        if (!entry.visible) {
            continue;
        }

        if (entry.panel) {
            entry.panel->update(game, entity);
        }

        if (entry.func) {
            entry.func(game, entity);
        }
    }

    ImGui::End();
}

//  ----------------------------------------------------------------------------
void EditorSystem::update_system_manager_gui(Game& game) {
    //  Build entity debug infos
    m_entity_infos.clear();
    build_entity_infos(game, m_entity_infos);

    if (m_entity_infos.empty()) {
        ImGui::Text("No entities.");
        return;
    }

    //  Listbox item getter
    auto entity_getter = [](
        void* vector,
        int index,
        const char** out_text
    ) {
        auto& infos = *static_cast<std::vector<EntityInfo>*>(vector);

        if (index < 0 || index >= static_cast<int>(infos.size())) {
            return false;
        }

        const EntityInfo& info = infos.at(index);
        *out_text = info.name.c_str();
        return true;
    };

    //  Listbox
    static int index = 0;
    ImGui::Text("Entities");
    ImGui::PushItemWidth(-1);
    ImGui::ListBox(
        "Entities",
        &index,
        entity_getter,
        static_cast<void*>(&const_cast<std::vector<EntityInfo>&>(m_entity_infos)),
        m_entity_infos.size()
    );
    ImGui::PopItemWidth();

    m_entity.set_entity(m_entity_infos.at(index).entity);

    ImGui::Separator();
}
}
