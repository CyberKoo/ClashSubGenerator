//
// Created by Kotarou on 2020/6/26.
//

#include <map>
#include <spdlog/spdlog.h>
#include <yaml-cpp/yaml.h>

#include "shadowsocksr_decoder.h"
#include "../uri.h"
#include "../utils.h"
#include "../exception/unsupported_configuration.h"

YAML::Node ShadowsocksRDecoder::decode_config(const Uri &uri) {
    spdlog::trace("using ssr decoder to decode config {}", uri.getHost());

    // decoding
    auto decoded_config = decode_base64(uri.getHost());
    spdlog::trace("decoded config {}", decoded_config);

    // parsing
    auto ssr = Utils::split(decoded_config, '/');
    auto main_config = Utils::split(ssr[0], ':');

    YAML::Node node(YAML::NodeType::Map);

    // additional parameters
    auto parameters = get_ssr_parameters(ssr[1]);
    if (!parameters["remarks"].empty()) {
        node["name"] = parameters["remarks"];
    } else {
        node["name"] = fmt::format("shadowsocksr_{}", Utils::get_random_string(10));
    }
    node["type"] = "ssr";
    node["server"] = main_config[0];
    node["port"] = main_config[1];
    node["protocol"] = main_config[2];
    node["cipher"] = main_config[3];
    node["obfs"] = main_config[4];
    node["password"] = decode_base64(main_config[5]);

    if (parameters.find("obfsparam") != parameters.end()) {
        node["obfs-param"] = parameters["obfsparam"];
    }

    if (parameters.find("protoparam") != parameters.end()) {
        node["protocol-param"] = parameters["protoparam"];
    }

    return node;
}

std::map<std::string, std::string> ShadowsocksRDecoder::get_ssr_parameters(std::string &query_string) {
    // remove leading ?
    if (query_string[0] == '?') {
        query_string.erase(0, 1);
    }

    std::map<std::string, std::string> parameters;
    auto parameter_pair = Utils::split(query_string, '&');
    for (const auto &pair : parameter_pair) {
        auto map_content = Utils::split(pair, '=');
        if (map_content.size() == 2) {
            auto value = decode_base64(map_content[1]);
            spdlog::trace("add parameter {} = {} to parameter map", map_content[0], value);
            parameters.emplace(map_content[0], value);
        } else {
            spdlog::debug("discard empty parameter {}", map_content[0]);
        }
    }

    return parameters;
}

std::string ShadowsocksRDecoder::decode_base64(std::string_view content) {
    auto s = Utils::replace_copy(content, {
                                         {"_", "/"},
                                         {"-", "+"}
                                 }
    );

    return ProxyDecoder::decode_base64(s);
}
