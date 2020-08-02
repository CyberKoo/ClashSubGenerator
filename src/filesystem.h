//
// Created by Kotarou on 2020/4/7.
//

#ifndef CLASHSUBGENERATOR_FILESYSTEM_H
#define CLASHSUBGENERATOR_FILESYSTEM_H

#include <string>

namespace FileSystem {
    bool exists(std::string_view path);

    bool mkdir(std::string_view path);

    void clear_directory(std::string_view path);
};


#endif //CLASHSUBGENERATOR_FILESYSTEM_H
