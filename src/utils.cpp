//
// Created by Kotarou on 2020/3/15.
//
#include <cctype>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>

#include <sys/stat.h>

#include "utils.h"

std::string Utils::str_tolower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return std::tolower(c); });
    return s;
}

std::vector<std::string> Utils::split(const std::string &s, char delim) {
    std::stringstream ss(s);
    std::string item;
    std::vector<std::string> elements;
    while (std::getline(ss, item, delim)) {
        elements.emplace_back(std::move(item));
    }
    return elements;
}

std::string Utils::get_time(const std::string &format, const std::time_t &timestamp) {
    char buffer[100];
    if (std::strftime(buffer, sizeof(buffer), format.c_str(), std::localtime(&timestamp))) {
        return buffer;
    }

    return "";
}

std::string Utils::get_time(const std::string &format) {
    return Utils::get_time(format, std::time(nullptr));
}

// from https://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring
// trim from start (in place)
void Utils::ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
        return !std::isspace(ch);
    }));
}

// trim from end (in place)
void Utils::rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

// trim from both ends (in place)
void Utils::trim(std::string &s) {
    Utils::ltrim(s);
    Utils::rtrim(s);
}

// trim from start (copying)
std::string Utils::ltrim_copy(std::string s) {
    Utils::ltrim(s);
    return s;
}

// trim from end (copying)
std::string Utils::rtrim_copy(std::string s) {
    Utils::rtrim(s);
    return s;
}

// trim from both ends (copying)
std::string Utils::trim_copy(std::string s) {
    Utils::trim(s);
    return s;
}

void Utils::replace(std::string &str, const std::map<std::string, std::string> &replace_list) {
    for (const auto &pair: replace_list) {
        auto pos = str.find(pair.first);
        while (pos != std::string::npos) {
            str.replace(pos, pair.first.length(), pair.second);
            pos = str.find(pair.first);
        }
    }
}