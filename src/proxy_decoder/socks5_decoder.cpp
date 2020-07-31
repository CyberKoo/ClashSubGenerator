//
// Created by Kotarou on 2020/6/26.
//

#include <fmt/format.h>
#include <yaml-cpp/yaml.h>

#include "socks5_decoder.h"
#include "../utils.h"
#include "../exception/unsupported_configuration.h"

YAML::Node Socks5Decoder::decode_config(std::string &content) {
    YAML::Node proxy = YAML::Node(YAML::NodeType::Map);

    auto [name, raw_config] = strip_name(content);
    auto decoded_config = decode_base64(raw_config);
    auto credentials_pos = decoded_config.find('@');
    auto credentials = Utils::split(decoded_config.substr(0, credentials_pos), ':');

    proxy["type"] = std::string("socks5");
    proxy["name"] = name;
    if (name.empty()) {
        proxy["name"] = fmt::format("s5_{}", std::rand() % 9999);
    }

    if (credentials.size() == 2) {
        proxy["username"] = credentials[0];
        proxy["password"] = credentials[1];
    }

    auto server_config = Utils::split(decoded_config.substr(credentials_pos + 1, decoded_config.size() - 1), ':');
    if (server_config.size() == 2) {
        proxy["server"] = server_config[0];
        proxy["port"] = server_config[1];
    } else {
        throw UnsupportedConfiguration("Incorrect Socks5 config");
    }

    return proxy;
}
