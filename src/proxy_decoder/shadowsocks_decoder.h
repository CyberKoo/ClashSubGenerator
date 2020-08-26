//
// Created by Kotarou on 2020/6/26.
//

#ifndef CLASHSUBGENERATOR_SHADOWSOCKS_DECODER_H
#define CLASHSUBGENERATOR_SHADOWSOCKS_DECODER_H

#include "proxy_decoder.h"

class ShadowsocksDecoder : public ProxyDecoder {
public:
    ShadowsocksDecoder() = default;

    ~ShadowsocksDecoder() override = default;

    YAML::Node decode_config(const Uri &uri) override;

private:
    static std::map<std::string, std::string> parse_config(std::string_view config);

    static std::string find_extract(std::string &str, std::string_view delimiter, bool reverse);
};


#endif //CLASHSUBGENERATOR_SHADOWSOCKS_DECODER_H
