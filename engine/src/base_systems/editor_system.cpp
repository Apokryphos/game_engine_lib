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
    EcsRoot& ecs_root = game.get_ecs_root();
    m_entities = ecs_root.get_entities();

    if (m_entities.empty()) {
        ImGui::Text("No entities.");
        return;
    }

    //  Listbox item getter
    auto entity_getter = [](
        void* vector,
        int index,
        const char** out_text
    ) {
        auto& infos = *static_cast<std::vector<Entity>*>(vector);

        if (index < 0 || index >= static_cast<int>(infos.size())) {
            return false;
        }

        const Entity& entity = infos.at(index);
        *out_text = std::to_string(entity.id).c_str();
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
        static_cast<void*>(&const_cast<std::vector<Entity>&>(m_entities)),
        m_entities.size()
    );

    m_entity.set_entity(m_entities.at(index));
}
}
