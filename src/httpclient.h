//
// Created by Kotarou on 2020/3/15.
//

#ifndef CLASHSUBGENERATOR_HTTPCLIENT_H
#define CLASHSUBGENERATOR_HTTPCLIENT_H

#include <httplib.h>
#include "uri.h"

class HttpClient {
public:
    static std::shared_ptr<httplib::Client> connect(const Uri &uri);

    static std::string get(const std::string &uri);

private:
    static std::shared_ptr<httplib::Client> get_http_client(const std::string &host, int port);

    static std::shared_ptr<httplib::Client> get_https_client(const std::string &host, int port);

    static std::string get_ca_path();
};

#endif //CLASHSUBGENERATOR_HTTPCLIENT_H
