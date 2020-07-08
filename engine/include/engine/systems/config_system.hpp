#pragma once

#include "common/ini_config.hpp"
#include "common/system.hpp"
#include "engine/systems/base_system_ids.hpp"
#include "input/input_bind_map.hpp"
#include "platform/window_options.hpp"
#include <filesystem>
#include <string>

namespace engine
{
class ConfigSystem : public common::System
{
    using InputBindMap = input::InputBindMap;

    std::filesystem::path m_config_path;
    common::IniConfig m_ini_config;

    InputBindMap m_keyboard_binds;
    InputBindMap m_mouse_binds;
    InputBindMap m_gamepad_binds;

    void load_config();
    void save_config();

public:
    ConfigSystem(const std::string app_name);
    const InputBindMap& get_gamepad_binds() const;
    const InputBindMap& get_keyboard_binds() const;
    const InputBindMap& get_mouse_binds() const;

    static const common::SystemId Id = SYSTEM_ID_CONFIG;

    void load_input_bindings(
        const InputBindMap& default_keyboard_bind_map,
        const InputBindMap& default_mouse_bind_map,
        const InputBindMap& default_gamepad_bind_map
    );

    //  Loads window options from config file.
    //  Populate options argument with default values.
    void load_window_options(platform::WindowOptions& options);
    void save_window_options(const platform::WindowOptions& options);
};
}
