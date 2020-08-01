//
// Created by Kotarou on 2020/6/26.
//

#include <fmt/format.h>
#include <yaml-cpp/yaml.h>

#include "shadowsocks_decoder.h"
#include "../uri.h"
#include "../utils.h"
#include "../exception/unsupported_configuration.h"

YAML::Node ShadowsocksDecoder::decode_config(const Uri &uri) {
    YAML::Node proxy = YAML::Node(YAML::NodeType::Map);
    auto [name, raw_config] = strip_name(uri.getHost());
    auto decoded_config = decode_base64(raw_config);
    auto config_view = std::string_view(decoded_config);
    auto credentials_pos = decoded_config.find('@');

    proxy["type"] = std::string("ss");
    proxy["name"] = name;
    if (name.empty()) {
        proxy["name"] = fmt::format("shadowsocks_{}", Utils::get_random_string(10));
    }

    auto credentials = Utils::split(config_view.substr(0, credentials_pos), ':');
    if (credentials.size() != 2) {
        throw UnsupportedConfiguration("Incorrect Shadowsocks settings, missing cipher or password");
    }

    proxy["cipher"] = credentials[0];
    proxy["password"] = credentials[1];

    auto server_config = Utils::split(config_view.substr(credentials_pos + 1, config_view.size() - 1), ':');
    if (server_config.size() == 2) {
        proxy["server"] = server_config[0];
        proxy["port"] = server_config[1];
    } else {
        throw UnsupportedConfiguration("Incorrect Shadowsocks config, missing server or port");
    }

    return proxy;
}
