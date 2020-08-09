//
// Created by Kotarou on 2020/6/26.
//

#ifndef CLASHSUBGENERATOR_PROXY_DECODER_H
#define CLASHSUBGENERATOR_PROXY_DECODER_H

#include <string>
#include <yaml-cpp/node/node.h>

class Uri;

class ProxyDecoder {
public:
    virtual ~ProxyDecoder() = default;

    virtual YAML::Node decode_config(const Uri &uri) = 0;

protected:
    static std::string decode_base64(std::string_view data);

    static std::pair<std::string, std::string> strip_name(std::string_view content);

protected:
    YAML::Node proxy;
};


#endif //CLASHSUBGENERATOR_PROXY_DECODER_H
