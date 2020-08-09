//
// Created by Kotarou on 2020/3/15.
//

#ifndef CLASHSUBGENERATOR_HTTPCLIENT_H
#define CLASHSUBGENERATOR_HTTPCLIENT_H

// forward declaration
namespace httplib {
    class Client;
}

class Uri;

class HttpClient {
public:
    static std::unique_ptr<httplib::Client> connect(const Uri &uri);

    static std::string get(std::string_view uri);

    static std::string get(const Uri &uri);

private:
    static std::string get_ca_path();

    static std::string get_user_agent();
};

#endif //CLASHSUBGENERATOR_HTTPCLIENT_H
