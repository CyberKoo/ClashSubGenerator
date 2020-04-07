//
// Created by Kotarou on 2020/3/15.
//

#ifndef CLASHSUBGENERATOR_UTILS_H
#define CLASHSUBGENERATOR_UTILS_H

#include <map>
#include <ctime>
#include <vector>

namespace Utils {
    void ltrim(std::string &s);

    void rtrim(std::string &s);

    void trim(std::string &s);

    std::string ltrim_copy(std::string s);

    std::string rtrim_copy(std::string s);

    std::string trim_copy(std::string s);

    std::string str_tolower(std::string s);

    std::string get_time(const std::string &format, const std::time_t &timestamp);

    std::string get_time(const std::string &format);

    std::vector<std::string> split(const std::string &s, char delim);

    void replace(std::string &str, const std::map<std::string, std::string> &replace_list);
}

#endif //CLASHSUBGENERATOR_UTILS_H
