//
// Created by Kotarou on 2020/3/15.
//

#ifndef CLASHSUBGENERATOR_UTILS_H
#define CLASHSUBGENERATOR_UTILS_H

#include <vector>

namespace Utils {
    std::string str_tolower(std::string s);

    bool file_exists(const std::string &name);

    std::vector<std::string> split(const std::string &s, char delim);
}


#endif //CLASHSUBGENERATOR_UTILS_H
