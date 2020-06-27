//
// Created by Kotarou on 2020/6/26.
//

#ifndef CLASHSUBGENERATOR_SOCKS5_DECODER_H
#define CLASHSUBGENERATOR_SOCKS5_DECODER_H

#include "proxy_decoder.h"

class Socks5Decoder : public ProxyDecoder {
public:
    YAML::Node decode_config(std::string &content) override;
};


#endif //CLASHSUBGENERATOR_SOCKS5_DECODER_H
