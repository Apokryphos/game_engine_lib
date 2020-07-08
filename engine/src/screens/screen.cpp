#include "common/log.hpp"
#include "engine/screens/screen.hpp"
#include <cassert>

using namespace common;

namespace engine
{
//  ----------------------------------------------------------------------------
void Screen::activate(Game& game) {
    //  Load
    if (m_state == State::None) {
        load(game);
    }

    //  Activate
    if (m_state == State::Loaded) {
        log_debug("Activating screen '%s'...", m_name.c_str());
        m_state = State::Activated;
        on_activate(game);
    }
}

//  ----------------------------------------------------------------------------
void Screen::deactivate(Game& game) {
    assert(m_state != State::None);

    if (m_state == State::Activated) {
        log_debug("Deactivating screen '%s'...", m_name.c_str());
        m_state = State::Loaded;
        on_deactivate(game);
    }
}

//  ----------------------------------------------------------------------------
void Screen::load(Game& game) {
    assert(m_state == State::None);

    log_debug("Loading screen '%s'...", m_name.c_str());
    m_state = State::Loaded;
    on_load(game);
}

//  ----------------------------------------------------------------------------
void Screen::render(Game& game) {
    assert(m_state == State::Activated);
    on_render(game);
}

//  ----------------------------------------------------------------------------
void Screen::update(Game& game) {
    assert(m_state == State::Activated);
    on_update(game);
}
}
