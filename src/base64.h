//
// Created by Kotarou on 2020/3/19.
//

#ifndef CLASHSUBGENERATOR_BASE64_H
#define CLASHSUBGENERATOR_BASE64_H

#include <string>

namespace Base64 {
    using container_type = std::vector<unsigned char>;

    std::string encode(const std::string &data);

    container_type decode(const std::string &data);

    std::string to_string(const Base64::container_type &result);
};

#endif //CLASHSUBGENERATOR_BASE64_H
