//
// Created by Kotarou on 2020/6/26.
//

#ifndef CLASHSUBGENERATOR_VMESS_DECODER_H
#define CLASHSUBGENERATOR_VMESS_DECODER_H

#include <unordered_map>
#include "proxy_decoder.h"

class VmessDecoder : public ProxyDecoder {
public:
    VmessDecoder();

    ~VmessDecoder() override = default;

    YAML::Node decode_config(const Uri &uri) override;

private:
    void network_ws();

    void network_tcp();

private:
    YAML::Node v2ray_config;

    std::unordered_map<std::string_view, std::string_view> clash2vmess;

    std::unordered_map<std::string_view, std::function<void()>> network_mapper;
};


#endif //CLASHSUBGENERATOR_VMESS_DECODER_H
