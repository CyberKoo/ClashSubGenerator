//
// Created by Kotarou on 2020/4/7.
//

#ifndef CLASHSUBGENERATOR_HASH_H
#define CLASHSUBGENERATOR_HASH_H


#include <string>

namespace Hash {
    std::string md5(std::string_view str);

    std::string sha1(std::string_view str);

    std::string sha256(std::string_view str);
};


#endif //CLASHSUBGENERATOR_HASH_H
