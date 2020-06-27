//
// Created by Kotarou on 2020/6/26.
//

#include "socks5_decoder.h"
#include "../exception/unsupported_configuration.h"

YAML::Node Socks5Decoder::decode_config(std::string &content) {
    throw UnsupportedConfiguration("Socks5 is not supported yet");
}
