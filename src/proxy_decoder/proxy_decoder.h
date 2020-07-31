//
// Created by Kotarou on 2020/6/26.
//

#ifndef CLASHSUBGENERATOR_PROXY_DECODER_H
#define CLASHSUBGENERATOR_PROXY_DECODER_H

#include <string>
#include <yaml-cpp/node/node.h>

class ProxyDecoder {
public:
    virtual ~ProxyDecoder() = default;

    static YAML::Node decode(std::string &content);

    virtual YAML::Node decode_config(std::string &content) = 0;
protected:
    static std::string decode_base64(std::string &data);

    static std::pair<std::string, std::string> strip_protocol(std::string &uri);

    static std::pair<std::string, std::string> strip_name(const std::string &content);

private:
    static std::unique_ptr<ProxyDecoder> get_decoder(const std::string&);
};


#endif //CLASHSUBGENERATOR_PROXY_DECODER_H
