//
// Created by Kotarou on 2020/7/26.
//

#ifndef CLASHSUBGENERATOR_TROJAN_DECODER_H
#define CLASHSUBGENERATOR_TROJAN_DECODER_H


#include "proxy_decoder.h"

class TrojanDecoder : public ProxyDecoder {
public:
    TrojanDecoder() = default;

    ~TrojanDecoder() override = default;

    YAML::Node decode_config(std::string &content) override;

    static std::map<std::string, std::string> get_parameters(std::string &query_string);
};


#endif //CLASHSUBGENERATOR_TROJAN_DECODER_H
