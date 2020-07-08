#include "engine/base_systems/debug_gui_system.hpp"

using namespace common;

namespace engine
{
//  ----------------------------------------------------------------------------
DebugGuiSystem::DebugGuiSystem()
: System(DebugGuiSystem::Id, "debug_gui_system") {
}

//  ----------------------------------------------------------------------------
void DebugGuiSystem::add_gui(const SystemId system_id, DebugGuiFunc func) {
    if (m_entries.find(system_id) != m_entries.end()) {
        throw std::runtime_error("Only one debug GUI function per system can be added.");
    }

    Entry entry{};
    entry.visible = true;
    entry.func = func;

    m_entries[system_id] = entry;
}

//  ----------------------------------------------------------------------------
void DebugGuiSystem::update(Game& game) {
    for (const auto& pair : m_entries) {
        const DebugGuiSystem::Entry& entry = pair.second;

        if (!entry.visible) {
            continue;
        }

        if (entry.func) {
            entry.func(game);
        }
    }
}
}
