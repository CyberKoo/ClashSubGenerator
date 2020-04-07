//
// Created by Kotarou on 2020/4/7.
//

#ifndef CLASHSUBGENERATOR_FILESYSTEM_H
#define CLASHSUBGENERATOR_FILESYSTEM_H


#include <string>

namespace FileSystem {
    bool exists(const std::string &path);

    bool mkdir(const std::string &path);

    void clear_directory(const std::string &path);
};


#endif //CLASHSUBGENERATOR_FILESYSTEM_H
