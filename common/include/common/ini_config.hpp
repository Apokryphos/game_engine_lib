#pragma once

#include "common/log.hpp"
#include "common/string.hpp"
#include <istream>
#include <map>
#include <string>
#include <vector>

namespace common
{
//  Parses an .ini config from a stream.
//  Comments can start with a semicolon or hash.
//  Trailing comments are not supported.
class IniConfig
{
    typedef std::map<std::string, std::string> SectionMap;

    std::map<std::string, SectionMap> m_sections;

    //  A string is stored for each line parsed.
    //  If a string is blank, there was no comment on that line.
    //  Otherwise the comment should be written when the config is saved.
    std::vector<std::string> m_comments;

    template <typename T>
    T convert_value_string(
        const std::string& value_str,
        const T default_value
    ) const {
        T value;
        if (std::istringstream(value_str) >> value) {
            //  Success
            return value;
        }

        return default_value;
    }

    template <typename T>
    T get_value(
        const std::string& section,
        const std::string& var,
        const T default_value
    ) const {
        //  Check if section and variable exist
        if (!exists(section, var)) {
            return default_value;
        }

        //  Get value string
        std::string value_str = m_sections.at(section).at(var);
        return convert_value_string<T>(value_str, default_value);
    }

    template <typename T>
    void set_value(
        const std::string& section,
        const std::string& var,
        T value
    ) {
        m_sections[section][var] = trim_copy(std::to_string(value));
    }

    void log_line(int line_num) {
        log_error("Error reading .ini file at line %d.", line_num);
    }

public:
    bool exists(const std::string& section, const std::string& var) const {
        //  Check if section exists
        if (m_sections.find(section) == m_sections.end()) {
            return false;
        }

        //  Check if variable exists
        const auto& section_map = m_sections.at(section);
        if (section_map.find(var) == section_map.end()) {
            return false;
        }

        return true;
    }

    bool parse(std::istream& stream) {
        //  Clear sections and comments
        m_sections.clear();
        m_comments.clear();

        std::string section = "";

        int line_num = 0;
        std::string line;
        while (std::getline(stream, line)) {
            ++line_num;

            trim(line);

            //  Ignore empty lines
            if (line.empty()) {
                m_comments.push_back("");
                continue;
            }

            //  Preserve comments
            if (line[0] == ';' || line[0] == '#') {
                m_comments.push_back(line);
                continue;
            } else {
                m_comments.push_back("");
            }

            //  Section '[section]'
            if (line[0] == '[') {
                if (line.back() != ']') {
                    log_error("Section is not enclosed in brackets.");
                    log_line(line_num);
                    return false;
                }

                line.erase(line.begin());
                line.erase(line.end() - 1);

                if (line.empty()) {
                    log_error("Section is empty.");
                    log_line(line_num);
                    return false;
                }

                section = line;

                continue;
            }

            //  Variable-value pair
            const size_t equal_pos = line.find('=');
            if (equal_pos != std::string::npos) {
                std::string var = line.substr(0, equal_pos);
                trim(var);
                if (var.empty()) {
                    log_error("Variable name missing");
                    log_line(line_num);
                    return false;
                }

                std::string value = line.substr(equal_pos + 1);
                trim(value);
                if (value.empty()) {
                    log_error("Value missing");
                    log_line(line_num);
                    return false;
                }

                if (exists(section, var)) {
                    log_error("Variable cannot be assigned more than once.");
                    log_line(line_num);
                    return false;
                }

                m_sections[section][var] = value;

                continue;
            }

            //  Failed to parse line
            log_line(line_num);

            return false;
        }

        return true;
    }

    bool get_bool(
        const std::string& section,
        const std::string& var,
        const bool default_value
    ) const {
        return get_value<bool>(section, var, default_value);
    }

    double get_double(
        const std::string& section,
        const std::string& var,
        const double default_value
    ) const {
        return get_value<double>(section, var, default_value);
    }

    float get_float(
        const std::string& section,
        const std::string& var,
        const float default_value
    ) const {
        return get_value<float>(section, var, default_value);
    }

    int get_int(
        const std::string& section,
        const std::string& var,
        const int default_value
    ) const {
        return get_value<int>(section, var, default_value);
    }

    std::string get_string(
        const std::string& section,
        const std::string& var,
        const std::string default_value
    ) const {
        return get_value<std::string>(section, var, default_value);
    }

    void save(std::ostream& stream) const;

    void set_bool(
        const std::string& section,
        const std::string& var,
        bool value
    );

    void set_double(
        const std::string& section,
        const std::string& var,
        double value
    );

    void set_float(
        const std::string& section,
        const std::string& var,
        float value
    );

    void set_int(
        const std::string& section,
        const std::string& var,
        int value
    );

    void set_string(
        const std::string& section,
        const std::string& var,
        std::string value
    );
};

template <>
inline bool IniConfig::convert_value_string(
    const std::string& value_str,
    const bool default_value
) const {
    //  Convert to lowercase for boolean strings
    const std::string value = to_lowercase_copy(value_str);

    if (value == "true" || value == "yes" || value == "1") {
        return true;
    }

    if (value == "false" || value == "no" || value == "0") {
        return false;
    }

    return default_value;
}

template <>
inline void IniConfig::set_value(
    const std::string& section,
    const std::string& var,
    std::string value
) {
    m_sections[section][var] = trim_copy(value);
}
}
