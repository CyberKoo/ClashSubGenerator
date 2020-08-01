//
// Created by Kotarou on 2020/6/26.
//

#ifndef CLASHSUBGENERATOR_SOCKS5_DECODER_H
#define CLASHSUBGENERATOR_SOCKS5_DECODER_H

#include "proxy_decoder.h"

class Socks5Decoder : public ProxyDecoder {
public:
    Socks5Decoder() = default;

    ~Socks5Decoder() override = default;

    YAML::Node decode_config(const Uri &uri) override;
};


#endif //CLASHSUBGENERATOR_SOCKS5_DECODER_H
