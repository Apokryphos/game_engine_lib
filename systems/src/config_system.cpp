#include "common/log.hpp"
#include "filesystem/paths.hpp"
#include "input/strings/input_strings.hpp"
#include "input/strings/input_action_strings.hpp"
#include "platform/window_options.hpp"
#include "systems/config_system.hpp"
#include "systems/system_ids.hpp"
#include <fstream>

namespace fs = std::filesystem;

using namespace common;
using namespace filesystem;
using namespace input;
using namespace platform;

namespace systems
{
//  ----------------------------------------------------------------------------
ConfigSystem::ConfigSystem(const std::string app_name)
: System(ConfigSystem::Id, "config_system") {
    const fs::path base_path = get_game_base_path(app_name);
    m_config_path = base_path / "config.ini";
}

//  ----------------------------------------------------------------------------
const InputBindMap& ConfigSystem::get_gamepad_binds() const {
    return m_gamepad_binds;
}

//  ----------------------------------------------------------------------------
const InputBindMap& ConfigSystem::get_keyboard_binds() const {
    return m_keyboard_binds;
}

//  ----------------------------------------------------------------------------
const InputBindMap& ConfigSystem::get_mouse_binds() const {
    return m_mouse_binds;
}

//  ----------------------------------------------------------------------------
void ConfigSystem::load_config() {
    if (fs::exists(m_config_path)) {
        log_info("Loading config '%s'...", m_config_path.c_str());
        std::ifstream file(m_config_path);
        m_ini_config.parse(file);
    } else {
        log_info("Could not find config.ini.");
    }
}

//  ----------------------------------------------------------------------------
void ConfigSystem::load_input_bindings(
    const InputBindMap& default_keyboard_bind_map,
    const InputBindMap& default_mouse_bind_map,
    const InputBindMap& default_gamepad_bind_map
) {
    log_debug("Loading input bindings...");

    //  Save default binds
    m_keyboard_binds = default_keyboard_bind_map;
    m_mouse_binds = default_mouse_bind_map;
    m_gamepad_binds = default_gamepad_bind_map;

    const auto& action_entries = get_input_action_strings();

    //  Check if each action exists in the config file
    for (const auto& action_entry : action_entries) {
        //  Read the name of the key assigned to this action
        const std::string config_name = m_ini_config.get_string(
            "input",
            action_entry.config_name,
            ""
        );

        if (config_name.empty()) {
            continue;
        }

        const std::vector<std::string> tokens = split(config_name, ',');
        for (const std::string& token : tokens) {
            //  Look up the entry for the key using the name specified in the config file
            //  e.g. "key.escape"
            const auto& entry = get_input_string(token);

            if (entry != nullptr) {
                const int input = entry->input;

                //  Bind the input
                if (entry->source == InputSource::Key) {
                    m_keyboard_binds.bind_key(action_entry.action_id, input);
                } else if (entry->source == InputSource::MouseButton) {
                    m_mouse_binds.bind_button(action_entry.action_id, input);
                } else if (entry->source == InputSource::Button) {
                    m_gamepad_binds.bind_button(action_entry.action_id, input);
                } else if (entry->source == InputSource::Axis) {
                    m_gamepad_binds.bind_axis(action_entry.action_id, input, entry->axis_sign);
                }

                const std::string& input_name = entry->name;
                log_debug(
                    "Bound input '%s' to input action '%s'.",
                    input_name.c_str(),
                    action_entry.name.c_str()
                );
            } else {
                log_error(
                    "Unknown input bind ignored ('%s' -> '%s').",
                    token.c_str(),
                    action_entry.config_name.c_str()
                );
            }
        }
    }
}

//  ----------------------------------------------------------------------------
void ConfigSystem::load_window_options(WindowOptions& options) {
    load_config();

    options.width = m_ini_config.get_int("window", "width", options.width);
    options.height = m_ini_config.get_int("window", "height", options.height);
}

//  ----------------------------------------------------------------------------
void ConfigSystem::save_config() {
    std::stringstream ss;
    m_ini_config.save(ss);

    std::ofstream file(m_config_path);
    file << ss.str();
    file.close();

    log_debug("Saved '%s'.", m_config_path.c_str());
}

//  ----------------------------------------------------------------------------
void ConfigSystem::save_window_options(const WindowOptions& options) {
    log_info("Saving window options to config.ini...");

    m_ini_config.set_int("window", "width", options.width);
    m_ini_config.set_int("window", "height", options.height);

    save_config();
}
}
