//
// Created by Kotarou on 2020/4/5.
//

#include <spdlog/spdlog.h>

#include "httpclient.h"
#include "base64.h"
#include "utils.h"
#include "shadowsocksr_subscriber.h"
#include "exception/unsupported_configuration.h"

void ShadowsocksRSubscriber::load(const std::string &uri) {
    throw UnsupportedConfiguration("ShadowsocksR is reserved for future use");

    auto config = HttpClient::get(uri);
    auto proxy_list = decode_config(config);
    auto raw_config = transform_config(proxy_list);
}

std::vector<std::string> ShadowsocksRSubscriber::decode_config(const std::string &config) {
    auto decoded = base64::decode(config);
    std::string decoded_str(decoded.begin(), decoded.end());

    auto proxy_list = Utils::split(decoded_str, '\n');
    for (auto &proxy: proxy_list) {
        Utils::replace(proxy, {
                               {"ssr://", ""},
                               {"_",      "/"},
                               {"-",      "+"}
                       }
        );
        Utils::trim(proxy);

        if (!proxy.empty()) {
            // decode again
            auto decoded_proxy = base64::decode(proxy);
            if (!decoded_proxy.empty()) {
                proxy = std::string(decoded_proxy.begin(), decoded_proxy.end());
            } else {
                spdlog::error("Failed to decode string {}", proxy);
                proxy.clear();
            }
        }
    }

    return proxy_list;
}

YAML::Node ShadowsocksRSubscriber::transform_config(const std::vector<std::string> &config) {
    YAML::Node proxies(YAML::NodeType::Sequence);

    for (const auto &proxy: config) {
        auto ssr = Utils::split(proxy, '/');
        auto exploded = Utils::split(ssr[0], ':');

        YAML::Node node(YAML::NodeType::Map);
        node["ip"] = exploded[0];
        node["port"] = exploded[1];
        node["protocol"] = exploded[2];
        node["method"] = exploded[3];
        node["obfuscate"] = exploded[4];
        auto password = base64::decode(exploded[5]);
        node["password"] = base64::to_string(password);

        // additional parameters
        node["parameters"] = get_parameters(ssr[1]);

        // push to list
        proxies.push_back(node);
    }

    return proxies;
}

std::map<std::string, std::string> ShadowsocksRSubscriber::get_parameters(std::string &query_string) {
    // remove leading ?
    if (query_string[0] == '?') {
        query_string.erase(0, 1);
    }

    std::map<std::string, std::string> parameters;
    auto parameter_pair = Utils::split(query_string, '&');
    for (const auto &pair : parameter_pair) {
        auto map_content = Utils::split(pair, '=');
        if (map_content.size() == 2) {
            auto value = base64::decode(map_content[1]);
            parameters.emplace(map_content[0], std::string(value.begin(), value.end()));
        } else {
            spdlog::warn("discard parameter {}", pair);
        }
    }

    return parameters;
}
