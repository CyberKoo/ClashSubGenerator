//
// Created by Kotarou on 2020/3/19.
//

#ifndef CLASHSUBGENERATOR_BASE64_H
#define CLASHSUBGENERATOR_BASE64_H

#include <string>

namespace Base64 {
    using container_t = std::vector<unsigned char>;

    std::string encode(std::string_view data);

    std::string decode(std::string_view data);
};

#endif //CLASHSUBGENERATOR_BASE64_H
