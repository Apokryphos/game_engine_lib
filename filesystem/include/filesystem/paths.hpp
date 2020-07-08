#pragma once

#include <filesystem>

namespace filesystem
{
std::filesystem::path get_game_base_path(const std::string& game_base_dir_name);
}
