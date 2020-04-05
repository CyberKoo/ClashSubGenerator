//
// Created by Kotarou on 2020/3/15.
//

#ifndef CLASHSUBGENERATOR_URI_H
#define CLASHSUBGENERATOR_URI_H

#include "exception/invalid_uri_exception.h"

class Uri {
public:
    static Uri Parse(const std::string &uri);

    const std::string &getQueryString() const;

    const std::string &getPath() const;

    const std::string &getProtocol() const;

    const std::string &getHost() const;

    int getPort() const;

private:
    std::string QueryString;
    std::string Path;
    std::string Protocol;
    std::string Host;
    int Port = 0;
};

#endif //CLASHSUBGENERATOR_URI_H
