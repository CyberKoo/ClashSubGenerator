//
// Created by Kotarou on 2020/6/26.
//

#include "shadowsocks_decoder.h"
#include "../exception/unsupported_configuration.h"

YAML::Node ShadowsocksDecoder::decode_config(std::string &content) {
    throw UnsupportedConfiguration("Shadowsocks is not supported yet");
}
