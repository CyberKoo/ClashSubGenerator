//
// Created by Kotarou on 2020/7/26.
//

#include <httplib.h>
#include <fmt/format.h>
#include <yaml-cpp/yaml.h>
#include <spdlog/spdlog.h>

#include "trojan_decoder.h"
#include "../uri.h"
#include "../utils.h"
#include "../exception/unsupported_configuration.h"

YAML::Node TrojanDecoder::decode_config(const Uri &uri) {
    YAML::Node proxy = YAML::Node(YAML::NodeType::Map);

    auto[name, raw_config] = strip_name(uri.getHost());
    auto queryString = uri.getQueryString();
    auto parameters = get_parameters(queryString);
    auto serverKey = Utils::split(uri.getHost(), '@');

    if (serverKey.size() != 2) {
        throw UnsupportedConfiguration("Incorrect Trojan config, missing password or server");
    }

    proxy["name"] = httplib::detail::decode_url(name, true);
    proxy["type"] = "trojan";
    proxy["server"] = serverKey[1];
    proxy["port"] = uri.getPort();
    proxy["password"] = serverKey[0];
    proxy["udp"] = true;

    if (parameters.count("sni")) {
        proxy["sni"] = parameters["sni"];
    }

    return proxy;
}

std::map<std::string, std::string> TrojanDecoder::get_parameters(std::string_view query_string) {
    std::map<std::string, std::string> parameters;
    auto parameter_pair = Utils::split(query_string, '&');
    for (const auto &pair : parameter_pair) {
        auto map_content = Utils::split(pair, '=');
        if (map_content.size() == 2) {
            spdlog::trace("add parameter {} = {} to parameter map", map_content[0], map_content[1]);
            parameters.emplace(map_content[0], map_content[1]);
        } else {
            spdlog::debug("discard empty parameter {}", map_content[0]);
        }
    }

    return parameters;
}