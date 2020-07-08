#pragma once

#include <algorithm>
#include <cstdio>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

namespace common
{
//  https://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring

//  ----------------------------------------------------------------------------
inline void replace(std::string& str, char ch, char r) {
    std::replace(
        str.begin(),
        str.end(),
        ch,
        r
    );
}

//  ----------------------------------------------------------------------------
template <typename... Args>
std::string stringf(const char* format, Args... args) {
    const size_t SIZE = std::snprintf(NULL, 0, format, args...);
    std::string buffer;
    buffer.resize(SIZE + 1);
    std::snprintf(&(buffer[0]), SIZE + 1, format, args...);
    if (buffer.size () > 0) {
        //  Remove trailing \0
        buffer.resize(buffer.size () - 1);
    }
    return std::move(buffer);
}

//  ----------------------------------------------------------------------------
inline void to_lowercase(std::string& str) {
    //  Convert to lowercase
    std::transform(
        str.begin(),
        str.end(),
        str.begin(),
        [](unsigned char c) {
            return std::tolower(c);
        }
    );
}

inline std::string to_lowercase_copy(std::string str) {
    //  Convert to lowercase
    std::transform(
        str.begin(),
        str.end(),
        str.begin(),
        [](unsigned char c) {
            return std::tolower(c);
        }
    );

    return str;
}

// trim from start (in place)
static inline void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
        return !std::isspace(ch);
    }));
}

// trim from end (in place)
static inline void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

// trim from both ends (in place)
static inline void trim(std::string &s) {
    ltrim(s);
    rtrim(s);
}

// trim from start (copying)
static inline std::string ltrim_copy(std::string s) {
    ltrim(s);
    return s;
}

// trim from end (copying)
static inline std::string rtrim_copy(std::string s) {
    rtrim(s);
    return s;
}

// trim from both ends (copying)
static inline std::string trim_copy(std::string s) {
    trim(s);
    return s;
}

//  String split functions: https://stackoverflow.com/a/236803
//  ----------------------------------------------------------------------------
template <typename Out>
void split(const std::string& s, char delim, Out result) {
    std::istringstream iss(s);
    std::string item;
    while (std::getline(iss, item, delim)) {
        *result++ = trim_copy(item);
    }
}

//  ----------------------------------------------------------------------------
inline std::vector<std::string> split(const std::string& s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, std::back_inserter(elems));
    return elems;
}

//  ----------------------------------------------------------------------------
//  Case insensitive compare
inline bool string_compare(std::string a, std::string b) {
    trim(a);
    to_lowercase(a);
    trim(b);
    to_lowercase(b);
    return a == b;
}
}
