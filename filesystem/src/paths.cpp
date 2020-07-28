#include "common/log.hpp"
#include <filesystem>

using namespace common;
namespace fs = std::filesystem;

namespace filesystem
{
//  ----------------------------------------------------------------------------
static void warn_xdg(bool found, fs::path path) {
    //  Warn user only once
    static bool displayed = false;

    if (displayed) {
        return;
    }

    //  Let user know if XDG_DATA_HOME is being used.
    //  If XDG_DATA_HOME is changed, save data may not show up anymore, so
    //  warn the user to help with troubleshooting.
    if (found) {
        log_info("Found $XDG_DATA_HOME. Using '%s' as home.", path.c_str());
    } else {
        log_info("$XDG_DATA_HOME not found. Using '%s' as home.", path.c_str());
    }

    displayed = true;
}

//  ----------------------------------------------------------------------------
static fs::path get_home_path() {
    fs::path path;

    bool found_xdg = false;
    const char* xdg_path = getenv("XDG_DATA_HOME");
    if (xdg_path != NULL) {
        path /= xdg_path;
        found_xdg = true;
    } else {
        //  Using C++ fs create_directories function with tilde in
        //  the path for home directory seems to create a subdirectory named '~'
        //  instead.
        const char* home_path = getenv("HOME");
        path /= home_path;
        path /= ".local";
        path /= "share";
    }

    warn_xdg(found_xdg, path);

    return path;
}

//  ----------------------------------------------------------------------------
fs::path get_game_base_path(const std::string& game_base_dir_name) {
    const fs::path base_path = get_home_path() / game_base_dir_name;

    if (!fs::exists(base_path)) {
        if (!fs::create_directories(base_path)) {
            throw std::runtime_error(
                "Could not create directory '" + base_path.string() + "'."
            );
        }

        log_info("Created save directory '%s'.", base_path.c_str());
    }

    return base_path;
}

//  ----------------------------------------------------------------------------
fs::path get_game_log_path(const std::string& game_base_dir_name) {
    const fs::path base_path = get_home_path() / game_base_dir_name;

    if (!fs::exists(base_path)) {
        if (!fs::create_directories(base_path)) {
            throw std::runtime_error(
                "Could not create directory '" + base_path.string() + "'."
            );
        }

        log_info("Created save directory '%s'.", base_path.c_str());
    }

    return base_path / "log.txt";
}
}
