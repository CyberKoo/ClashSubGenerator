//
// Created by Kotarou on 2020/6/26.
//

#ifndef CLASHSUBGENERATOR_SHADOWSOCKSR_DECODER_H
#define CLASHSUBGENERATOR_SHADOWSOCKSR_DECODER_H

#include <map>
#include "proxy_decoder.h"

class ShadowsocksRDecoder : public ProxyDecoder {
public:
    ShadowsocksRDecoder() = default;

    YAML::Node decode_config(std::string &content) override;

    static std::map<std::string, std::string> get_ssr_parameters(std::string &query_string);
};


#endif //CLASHSUBGENERATOR_SHADOWSOCKSR_DECODER_H
