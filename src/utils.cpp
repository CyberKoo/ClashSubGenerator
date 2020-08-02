//
// Created by Kotarou on 2020/3/15.
//
#include <httplib.h>
#include <cctype>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <random>

#include "utils.h"

std::string Utils::get_random_string(unsigned int length) {
    constexpr std::string_view CHARACTER_SET = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    std::random_device seed;
    std::default_random_engine engine(seed());
    std::uniform_int_distribution<int> distribution(0, CHARACTER_SET.size());

    std::ostringstream oss;
    for (std::size_t i = 0; i < length; ++i) {
        oss << CHARACTER_SET[distribution(engine)];
    }

    return oss.str();
}

std::string Utils::str_tolower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return std::tolower(c); });
    return s;
}

std::vector<std::string> Utils::split(const std::string &s, const char delim) {
    return Utils::split(std::string_view(s), delim);
}

std::vector<std::string> Utils::split(std::string_view s, const char delim) {
    std::stringstream ss(s.data());
    std::string item;
    std::vector<std::string> elements;

    while (std::getline(ss, item, delim)) {
        elements.emplace_back(std::move(item));
    }

    return elements;
}

std::string Utils::get_time(std::string_view format, const std::time_t &timestamp) {
    char buffer[100];
    if (std::strftime(buffer, sizeof(buffer), format.data(), std::localtime(&timestamp))) {
        return buffer;
    }

    return "";
}

std::string Utils::get_time(std::string_view format) {
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

void Utils::replace(std::string &str, const std::map<std::string_view, std::string_view> &replace_list) {
    for (const auto &[target, new_content]: replace_list) {
        auto pos = str.find(target);
        while (pos != std::string::npos) {
            str.replace(pos, target.length(), new_content);
            pos = str.find(target);
        }
    }
}

std::string Utils::replace_copy(std::string_view str, const std::map<std::string_view, std::string_view> &replace_list) {
    auto s = std::string(str);
    Utils::replace(s, replace_list);

    return s;
}

std::string Utils::url_decode(const std::string &url_encoded, bool convert_plus_to_space) {
    return httplib::detail::decode_url(url_encoded, convert_plus_to_space);
}