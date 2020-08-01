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
};


#endif //CLASHSUBGENERATOR_SHADOWSOCKS_DECODER_H
