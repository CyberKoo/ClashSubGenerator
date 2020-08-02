//
// Created by Kotarou on 2020/3/15.
//

#ifndef CLASHSUBGENERATOR_UTILS_H
#define CLASHSUBGENERATOR_UTILS_H

#include <map>
#include <ctime>
#include <vector>

namespace Utils {
    std::string get_random_string(unsigned length);

    void ltrim(std::string &s);

    void rtrim(std::string &s);

    void trim(std::string &s);

    std::string ltrim_copy(std::string s);

    std::string rtrim_copy(std::string s);

    std::string trim_copy(std::string s);

    std::string str_tolower(std::string s);

    std::string get_time(std::string_view format, const std::time_t &timestamp);

    std::string get_time(std::string_view format);

    std::vector<std::string> split(const std::string &s, char delim);

    std::vector<std::string> split(std::string_view s, char delim);

    void replace(std::string &str, const std::map<std::string_view, std::string_view> &replace_list);

    std::string replace_copy(std::string_view str, const std::map<std::string_view, std::string_view> &replace_list);

    std::string url_decode(const std::string &url_encoded, bool convert_plus_to_space);
}

#endif //CLASHSUBGENERATOR_UTILS_H
