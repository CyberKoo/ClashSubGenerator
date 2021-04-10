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

VmessDecoder::VmessDecoder() {
    clash2vmess = {
            {"name",    "ps"},
            {"server",  "add"},
            {"port",    "port"},
            {"alterId", "aid"},
            {"uuid",    "id"}
    };

    network_mapper = {
            {"tcp", [this] { network_tcp(); }},
            {"ws",  [this] { network_ws(); }},
    };
}

YAML::Node VmessDecoder::decode_config(const Uri &uri) {
    auto decoded_config = decode_base64(uri.getBody());
    v2ray_config = YAML::Load(decoded_config);
    proxy = YAML::Node(YAML::NodeType::Map);

    // check version
    if (v2ray_config["v"].IsDefined() && v2ray_config["v"].IsScalar()) {
        if (v2ray_config["v"].as<int>() != 2) {
            throw UnsupportedConfiguration("only support v2rayN ver2 format");
        }
    }

    // check spoof type
    if (v2ray_config["type"].IsDefined() && v2ray_config["type"].IsScalar()) {
        if (auto spoof_type = v2ray_config["type"].as<std::string>(); spoof_type != "none") {
            throw UnsupportedConfiguration(fmt::format("Spoof type {} is not supported by clash yet", spoof_type));
        }
    }

    // automatic mapping according to the predefined map
    for (const auto &[clash, vmess]: clash2vmess) {
        // check key's existence and type
        if (v2ray_config[vmess.data()].IsDefined() && v2ray_config[vmess.data()].IsScalar()) {
            proxy[clash.data()] = Utils::trim_copy(v2ray_config[vmess.data()].as<std::string>());
        } else {
            throw MissingKeyException(fmt::format("Required key \"{}\" is missing", vmess));
        }
    }

    // check the network field
    if (v2ray_config["net"].IsDefined() && v2ray_config["net"].IsScalar()) {
        auto network_str = v2ray_config["net"].as<std::string>();
        Utils::str_tolower(network_str);

        if (network_mapper.find(network_str) != network_mapper.end()) {
            network_mapper.at(network_str)();
        } else {
            throw UnsupportedConfiguration(fmt::format("Network type \"{}\" is not supported by clash yet"));
        }
    } else {
        throw MissingKeyException(fmt::format("Required key \"{}\" is missing", "net"));
    }

    proxy["cipher"] = "auto";
    // vless maybe?
    proxy["type"] = "vmess";
    proxy["udp"] = true;

    // convert tls field
    if (v2ray_config["tls"].IsDefined() && v2ray_config["tls"].IsScalar()) {
        auto tls_str = v2ray_config["tls"].as<std::string>();
        Utils::str_tolower(tls_str);
        proxy["tls"] = (tls_str == "tls");
    }

    return proxy;
}

void VmessDecoder::network_ws() {
    proxy["network"] = "ws";
    if (v2ray_config["host"].IsDefined()) {
        if (v2ray_config["path"].IsScalar()) {
            proxy["ws-path"] = v2ray_config["path"];
        }
        proxy["ws-headers"] = YAML::Node(YAML::NodeType::Sequence);
        proxy["ws-headers"]["Host"] = v2ray_config["host"].as<std::string>();
    }
}

void VmessDecoder::network_tcp() {
    if (v2ray_config["host"].IsDefined() && v2ray_config["host"].size()) {
        auto opts = proxy["http-opts"];
        // proxy["network"] = "http";
        // opts["method"] = "GET";
        opts["headers"]["Host"].push_back(v2ray_config["host"]);
        if (v2ray_config["path"].IsScalar()) {
            auto http_paths = Utils::split(v2ray_config["path"].as<std::string>(), ',');
            for (const auto &path: http_paths) {
                opts["path"].push_back(Utils::trim_copy(path));
            }
        }
    }
}
