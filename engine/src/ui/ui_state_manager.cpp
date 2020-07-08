#include "common/log.hpp"
#include "engine/engine.hpp"
#include "engine/game.hpp"
#include "engine/ui/ui_state.hpp"
#include "engine/ui/ui_state_manager.hpp"
#include "input/input_manager.hpp"
#include <algorithm>
#include <stdexcept>

using namespace common;
using namespace input;

namespace engine
{
//  ----------------------------------------------------------------------------
UiStateManager::UiStateManager()
: m_active(nullptr) {
}

//  ----------------------------------------------------------------------------
UiStateManager::~UiStateManager() {
}

//  ----------------------------------------------------------------------------
void UiStateManager::activate(Game& game, const std::string& state_name) {
    UiState* state = find_state(state_name);

    if (state == nullptr) {
        throw std::runtime_error("UI state not found.");
    }

    //  Deactivate currently active UI state
    if (m_active != nullptr) {
        m_active->deactivate(game);
    }

    m_active = state;

    //  Activate new screen
    m_active->activate(game);
}

//  ----------------------------------------------------------------------------
void UiStateManager::add_state(Game& game, std::unique_ptr<UiState> state) {
    if (find_state(state->get_name()) != nullptr) {
        throw std::runtime_error("A UI state with the same name already exists.");
    }

    UiState* state_ptr = state.get();

    m_states.push_back(std::move(state));

    log_debug("Added UI state '%s'.", state_ptr->get_name().c_str());
}

//  ----------------------------------------------------------------------------
UiState* UiStateManager::find_state(const std::string& state_name) {
    auto find = std::find_if(
        m_states.begin(),
        m_states.end(),
        [&state_name](const std::unique_ptr<UiState>& state) {
            return state->get_name() == state_name;
        }
    );

    if (find == m_states.end()) {
        return nullptr;
    }

    return (*find).get();
}

//  ----------------------------------------------------------------------------
void UiStateManager::process_event(Game& game, const InputEvent& event) {
    if (m_active == nullptr) {
        throw std::runtime_error("No active UI state to update.");
    }

    m_active->process_event(game, event);
}
}
