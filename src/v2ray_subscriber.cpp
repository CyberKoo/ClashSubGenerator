//
// Created by Kotarou on 2020/3/19.
//

#include <spdlog/spdlog.h>

#include "base64.h"
#include "utils.h"
#include "httpclient.h"
#include "v2ray_subscriber.h"
#include "exception/missing_key_exception.h"
#include "exception/unsupported_configuration.h"

void V2raySubscriber::load(const std::string &uri) {
    auto config = HttpClient::get(uri);
    auto remote_proxies = decode_config(config);
    proxies = YAML::Node(YAML::NodeType::Sequence);

    for (const auto &proxy_config: remote_proxies) {
        try {
            if (!proxy_config.empty()) {
                proxies.push_back(generate_proxy(proxy_config));
            }
        } catch (CSGRuntimeException &e) {
            spdlog::warn("Skipped adding proxy {}, due to {}", proxy_config, e.what());
        }
    }

    if (proxies.size() > 0) {
        spdlog::info("Total number of proxies loaded {}", proxies.size());
    } else {
        spdlog::warn("No proxy loaded, this is probably not what you expected", proxies.size());
    }
}

std::vector<std::string> V2raySubscriber::decode_config(const std::string &config) {
    auto decoded = Base64::decode(config);
    std::string decoded_str(decoded.begin(), decoded.end());

    auto proxy_list = Utils::split(decoded_str, '\n');
    for (auto &proxy: proxy_list) {
        // strip vmess:// prefix
        Utils::replace(proxy, {{"vmess://", ""}});
        Utils::trim(proxy);

        if (!proxy.empty()) {
            // decode again
            auto decoded_proxy = Base64::decode(proxy);
            if (!decoded_proxy.empty()) {
                proxy = Base64::to_string(decoded_proxy);
            } else {
                spdlog::error("Failed to decode string {}", proxy);
                proxy.clear();
            }
        }
    }

    return proxy_list;
}

YAML::Node V2raySubscriber::generate_proxy(const std::string &config) {
    auto yaml = YAML::Load(config);
    auto proxy = YAML::Node(YAML::NodeType::Map);
    const std::map<std::string, std::string> vmess2clash = {
            {"name",    "ps"},
            {"server",  "add"},
            {"port",    "port"},
            {"alterId", "aid"},
            {"uuid",    "id"}
    };

    // check spoof type
    if (yaml["type"].IsDefined() && yaml["type"].IsScalar()) {
        auto spoof_type = yaml["type"].as<std::string>();
        if (spoof_type != "none") {
            throw UnsupportedConfiguration(fmt::format("Spoof type {} is not supported by clash yet", spoof_type));
        }
    }

    // check the network field
    if (yaml["net"].IsDefined() && yaml["net"].IsScalar()) {
        auto network_str = yaml["net"].as<std::string>();
        Utils::str_tolower(network_str);

        if (network_str == "ws" || network_str == "tcp") {
            // do i need ws-headers?
            if (network_str == "ws" && yaml["host"].IsDefined()) {
                proxy["network"] = "ws";
                if (yaml["path"].IsScalar()) {
                    proxy["ws-path"] = yaml["path"];
                }
                proxy["ws-headers"] = YAML::Node(YAML::NodeType::Sequence);
                proxy["ws-headers"]["Host"] = yaml["host"].as<std::string>();
            }
        } else {
            throw UnsupportedConfiguration(fmt::format("Network type \"{}\" is not supported by clash yet"));
        }
    } else {
        throw MissingKeyException(fmt::format("Required key \"{}\" is missing", "net"));
    }

    // automatic mapping according to the predefined map
    for (const auto &pair: vmess2clash) {
        // check key's existence and type
        if (yaml[pair.second].IsDefined() && yaml[pair.second].IsScalar()) {
            proxy[pair.first] = Utils::trim_copy(yaml[pair.second].as<std::string>());
        } else {
            throw MissingKeyException(fmt::format("Required key \"{}\" is missing", pair.second));
        }
    }

    proxy["cipher"] = "auto";
    proxy["type"] = "vmess";
    // convert tls field
    if (yaml["tls"].IsDefined() && yaml["tls"].IsScalar()) {
        auto tls_value = yaml["tls"].as<std::string>();
        Utils::str_tolower(tls_value);
        proxy["tls"] = (tls_value == "tls");
    }

    return proxy;
}
