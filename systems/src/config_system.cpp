#include "common/log.hpp"
#include "filesystem/paths.hpp"
#include "platform/window_options.hpp"
#include "systems/config_system.hpp"
#include "systems/system_ids.hpp"
#include <fstream>

namespace fs = std::filesystem;

using namespace common;
using namespace filesystem;
using namespace platform;

namespace systems
{
//  ----------------------------------------------------------------------------
ConfigSystem::ConfigSystem(const std::string app_name)
: System(SYSTEM_ID_CONFIG, "config_system") {
    const fs::path base_path = get_game_base_path(app_name);
    m_config_path = base_path / "config.ini";
}

//  ----------------------------------------------------------------------------
void ConfigSystem::load_config() {
    if (fs::exists(m_config_path)) {
        log_info("Loading config '%s'...", m_config_path.c_str());
        std::ifstream file(m_config_path);
        m_config.parse(file);
    } else {
        log_info("Could not find config.ini.");
    }
}

//  ----------------------------------------------------------------------------
void ConfigSystem::load_window_options(WindowOptions& options) {
    load_config();

    options.width = m_config.get_int("window", "width", options.width);
    options.height = m_config.get_int("window", "height", options.height);
}

//  ----------------------------------------------------------------------------
void ConfigSystem::save_config() {
    std::stringstream ss;
    m_config.save(ss);

    std::ofstream file(m_config_path);
    file << ss.str();
    file.close();

    log_debug("Saved '%s'.", m_config_path.c_str());
}

//  ----------------------------------------------------------------------------
void ConfigSystem::save_window_options(const WindowOptions& options) {
    log_info("Saving window options to config.ini...");

    m_config.set_int("window", "width", options.width);
    m_config.set_int("window", "height", options.height);

    save_config();
}
}
