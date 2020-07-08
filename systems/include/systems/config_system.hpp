#pragma once

#include "common/ini_config.hpp"
#include "common/system.hpp"
#include "platform/window_options.hpp"
#include <filesystem>
#include <string>

namespace systems
{
class ConfigSystem : public common::System
{
    std::filesystem::path m_config_path;
    common::IniConfig m_config;

    void load_config();
    void save_config();

public:
    ConfigSystem(const std::string app_name);

    //  Loads window options from config file.
    //  Populate options argument with default values.
    void load_window_options(platform::WindowOptions& options);
    void save_window_options(const platform::WindowOptions& options);
};
}
