//
// Created by Kotarou on 2020/6/26.
//

#ifndef CLASHSUBGENERATOR_VMESS_DECODER_H
#define CLASHSUBGENERATOR_VMESS_DECODER_H

#include "proxy_decoder.h"

class VmessDecoder : public ProxyDecoder {
public:
    VmessDecoder() = default;

    ~VmessDecoder() override = default;

    YAML::Node decode_config(std::string &content) override;

private:
    YAML::Node config;
    YAML::Node proxy;
};


#endif //CLASHSUBGENERATOR_VMESS_DECODER_H
