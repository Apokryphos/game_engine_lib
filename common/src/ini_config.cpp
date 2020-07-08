#include "common/ini_config.hpp"

namespace common
{
//  ----------------------------------------------------------------------------
static void write_remaining_comments(
    std::vector<std::string>& comments,
    std::ostream& stream
) {
    for (const std::string& comment : comments) {
        if (!comment.empty()) {
            //  Write comment
            stream << comment << "\n";
        }
    }

    comments.clear();
}

//  ----------------------------------------------------------------------------
static void write_comments(
    std::vector<std::string>& comments,
    std::ostream& stream
) {
    if (comments.empty()) {
        return;
    }

    //  Write comments until a blank entry is encountered
    while (!comments.empty()) {
        const std::string comment = comments.front();

        //  Remove comment from list
        comments.erase(comments.begin());

        if (comment.empty()) {
            return;
        }

        //  Write comment
        stream << comment << "\n";
    }
}

//  ----------------------------------------------------------------------------
void IniConfig::save(std::ostream& stream) const {
    //  Make a copy of comments
    auto comments = m_comments;

    for (const auto& section_pair : m_sections) {
        //  Write comments between sections
        write_comments(comments, stream);

        //  Write section
        const std::string section = section_pair.first;
        stream << "[" << section << "]\n";

        //  Write comments between sections and variable/value pairs
        write_comments(comments, stream);

        //  Write variable and value pairs
        for (const auto& var_pair : section_pair.second) {
            const std::string var = var_pair.first;
            const std::string value = var_pair.second;

            stream << var << "=" << value << "\n";

            write_comments(comments, stream);
        }

        //  Write a blank line inbetween sections
        stream << "\n";
    }

    //  Write any trailing comments
    write_remaining_comments(comments, stream);
}

//  ----------------------------------------------------------------------------
void IniConfig::set_bool(
    const std::string& section,
    const std::string& var,
    bool value
) {
    set_value<std::string>(section, var, value ? "true" : "false");
}

//  ----------------------------------------------------------------------------
void IniConfig::set_double(
    const std::string& section,
    const std::string& var,
    double value
) {
    set_value<double>(section, var, value);
}

//  ----------------------------------------------------------------------------
void IniConfig::set_float(
    const std::string& section,
    const std::string& var,
    float value
) {
    set_value<float>(section, var, value);
}

//  ----------------------------------------------------------------------------
void IniConfig::set_int(
    const std::string& section,
    const std::string& var,
    int value
) {
    set_value<int>(section, var, value);
}

//  ----------------------------------------------------------------------------
void IniConfig::set_string(
    const std::string& section,
    const std::string& var,
    std::string value
) {
    trim(value);
    set_value<std::string>(section, var, value);
}
}
