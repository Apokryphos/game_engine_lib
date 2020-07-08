#include "common/log.hpp"
#include "engine/ui/ui_state.hpp"
#include <cassert>

using namespace common;

namespace engine
{
//  ----------------------------------------------------------------------------
void UiState::activate(Game& game) {
    //  Load
    if (m_state == State::None) {
        load(game);
    }

    //  Activate
    if (m_state != State::Activated) {
        log_debug("Activating UI state '%s'...", m_name.c_str());
        m_state = State::Activated;
        on_activate(game);
    }
}

//  ----------------------------------------------------------------------------
void UiState::deactivate(Game& game) {
    assert(m_state != State::None);

    if (m_state == State::Activated) {
        log_debug("Deactivating UI state '%s'...", m_name.c_str());
        m_state = State::Loaded;
        on_deactivate(game);
    }
}

//  ----------------------------------------------------------------------------
void UiState::load(Game& game) {
    assert(m_state == State::None);

    log_debug("Loading UI state '%s'...", m_name.c_str());
    m_state = State::Loaded;
    on_load(game);
}

//  ----------------------------------------------------------------------------
void UiState::process_event(Game& game, const InputEvent& event) {
    assert(m_state == State::Activated);
    on_process_event(game, event);
}
}
