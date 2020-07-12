//
// Created by Kotarou on 2020/6/26.
//

#include <fmt/format.h>
#include <yaml-cpp/yaml.h>

#include "shadowsocks_decoder.h"
#include "../utils.h"
#include "../exception/unsupported_configuration.h"

YAML::Node ShadowsocksDecoder::decode_config(std::string &content) {
    YAML::Node proxy = YAML::Node(YAML::NodeType::Map);

    auto config_pair = strip_name(content);
    auto decoded_config = decode_base64(config_pair.second);
    auto credentials_pos = decoded_config.find('@');

    proxy["type"] = std::string("ss");
    proxy["name"] = config_pair.first;
    if (config_pair.first.empty()) {
        proxy["name"] = fmt::format("ss_{}", std::rand() % 9999);
    }

    auto credentials = Utils::split(decoded_config.substr(0, credentials_pos), ':');
    if (credentials.size() != 2) {
        throw UnsupportedConfiguration("Incorrect Shadowsocks settings, missing cipher or password");
    }

    proxy["cipher"] = credentials[0];
    proxy["password"] = credentials[1];

    auto config = Utils::split(decoded_config.substr(credentials_pos + 1, decoded_config.size() - 1), ':');
    if (config.size() == 2) {
        proxy["server"] = config[0];
        proxy["port"] = config[1];
    } else {
        throw UnsupportedConfiguration("Incorrect Shadowsocks settings, missing server or port");
    }

    return proxy;
}
