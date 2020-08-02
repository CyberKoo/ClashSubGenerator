//
// Created by Kotarou on 2020/3/15.
//

#ifndef CLASHSUBGENERATOR_HTTPCLIENT_H
#define CLASHSUBGENERATOR_HTTPCLIENT_H

#include "uri.h"

// forward declaration
namespace httplib {
    class ClientImpl;
}

class HttpClient {
public:
    static std::unique_ptr<httplib::ClientImpl> connect(const Uri &uri);

    static std::string get(std::string_view uri);

private:
    static std::unique_ptr<httplib::ClientImpl> get_http_client(std::string_view host, int port);

    static std::unique_ptr<httplib::ClientImpl> get_https_client(std::string_view host, int port);

    static std::string get_ca_path();

    static std::string get_user_agent();
};

#endif //CLASHSUBGENERATOR_HTTPCLIENT_H
