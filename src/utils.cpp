//
// Created by Kotarou on 2020/3/15.
//

#include <sstream>
#include <vector>
#include <algorithm>
#include <sys/stat.h>

#include "utils.h"

std::string Utils::str_tolower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return std::tolower(c); });
    return s;
}

bool Utils::file_exists(const std::string &name) {
    struct stat buffer{};
    return (stat(name.c_str(), &buffer) == 0);
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
