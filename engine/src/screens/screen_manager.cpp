#include "common/log.hpp"
#include "engine/screens/screen.hpp"
#include "engine/screens/screen_manager.hpp"
#include <algorithm>
#include <stdexcept>

using namespace common;

namespace engine
{
//  ----------------------------------------------------------------------------
ScreenManager::ScreenManager()
: m_active(nullptr) {
}

//  ----------------------------------------------------------------------------
ScreenManager::~ScreenManager() {
}

//  ----------------------------------------------------------------------------
void ScreenManager::activate(Game& game, const std::string& screen_name) {
    Screen* screen = find_screen(screen_name);

    if (screen == nullptr) {
        throw std::runtime_error("Screen not found.");
    }

    //  Deactivate currently active screen
    if (m_active != nullptr) {
        m_active->deactivate(game);
    }

    m_active = screen;

    //  Activate new screen
    m_active->activate(game);
}

//  ----------------------------------------------------------------------------
void ScreenManager::add_screen(Game& game, std::unique_ptr<Screen> screen) {
    if (find_screen(screen->get_name()) != nullptr) {
        throw std::runtime_error("A screen with the same name already exists.");
    }

    Screen* screen_ptr = screen.get();

    m_screens.push_back(std::move(screen));

    log_debug("Added screen '%s'.", screen_ptr->get_name().c_str());
}

//  ----------------------------------------------------------------------------
Screen* ScreenManager::find_screen(const std::string& screen_name) {
    auto find = std::find_if(
        m_screens.begin(),
        m_screens.end(),
        [&screen_name](const std::unique_ptr<Screen>& screen) {
            return screen->get_name() == screen_name;
        }
    );

    if (find == m_screens.end()) {
        return nullptr;
    }

    return (*find).get();
}

//  ----------------------------------------------------------------------------
void ScreenManager::render(Game& game) {
    if (m_active == nullptr) {
        throw std::runtime_error("No active screen to render.");
    }

    m_active->render(game);
}

//  ----------------------------------------------------------------------------
void ScreenManager::update(Game& game) {
    if (m_active == nullptr) {
        throw std::runtime_error("No active screen to update.");
    }

    m_active->update(game);
}
}
