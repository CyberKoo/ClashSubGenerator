//
// Created by Kotarou on 2020/6/26.
//
#include <fmt/format.h>
#include <yaml-cpp/yaml.h>

#include "vmess_decoder.h"
#include "../uri.h"
#include "../utils.h"
#include "../exception/unsupported_configuration.h"
#include "../exception/missing_key_exception.h"

YAML::Node VmessDecoder::decode_config(const Uri &uri) {
    auto decoded_config = decode_base64(fmt::format("{}{}", uri.getHost(), uri.getPath()));
    config = YAML::Load(decoded_config);
    proxy = YAML::Node(YAML::NodeType::Map);
    const std::map<std::string_view, std::string_view> clash2vmess = {
            {"name",    "ps"},
            {"server",  "add"},
            {"port",    "port"},
            {"alterId", "aid"},
            {"uuid",    "id"}
    };

    // check spoof type
    if (config["type"].IsDefined() && config["type"].IsScalar()) {
        auto spoof_type = config["type"].as<std::string>();
        if (spoof_type != "none") {
            throw UnsupportedConfiguration(fmt::format("Spoof type {} is not supported by clash yet", spoof_type));
        }
    }

    // check the network field
    if (config["net"].IsDefined() && config["net"].IsScalar()) {
        auto network_str = config["net"].as<std::string>();
        Utils::str_tolower(network_str);

        if (network_str == "ws" || network_str == "tcp") {
            // do i need ws-headers?
            if (network_str == "ws" && config["host"].IsDefined()) {
                proxy["network"] = "ws";
                if (config["path"].IsScalar()) {
                    proxy["ws-path"] = config["path"];
                }
                proxy["ws-headers"] = YAML::Node(YAML::NodeType::Sequence);
                proxy["ws-headers"]["Host"] = config["host"].as<std::string>();
            }
        } else {
            throw UnsupportedConfiguration(fmt::format("Network type \"{}\" is not supported by clash yet"));
        }
    } else {
        throw MissingKeyException(fmt::format("Required key \"{}\" is missing", "net"));
    }

    // automatic mapping according to the predefined map
    for (const auto &[clash, vmess]: clash2vmess) {
        // check key's existence and type
        if (config[vmess.data()].IsDefined() && config[vmess.data()].IsScalar()) {
            proxy[clash.data()] = Utils::trim_copy(config[vmess.data()].as<std::string>());
        } else {
            throw MissingKeyException(fmt::format("Required key \"{}\" is missing", vmess));
        }
    }

    proxy["cipher"] = "auto";
    proxy["type"] = "vmess";
    // convert tls field
    if (config["tls"].IsDefined() && config["tls"].IsScalar()) {
        auto tls_value = config["tls"].as<std::string>();
        Utils::str_tolower(tls_value);
        proxy["tls"] = (tls_value == "tls");
    }

    return proxy;
}
