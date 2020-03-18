//
// Created by Kotarou on 2020/3/15.
//

#ifndef CLASHSUBGENERATOR_URI_H
#define CLASHSUBGENERATOR_URI_H

#include "exception/invalid_uri_exception.h"

class Uri {
public:
    std::string QueryString, Path, Protocol, Host;
    int Port = 0;

    static Uri Parse(const std::string &uri);
};

#endif //CLASHSUBGENERATOR_URI_H
