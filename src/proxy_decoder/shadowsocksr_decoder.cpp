//
// Created by Kotarou on 2020/6/26.
//

#include <map>
#include <spdlog/spdlog.h>
#include "shadowsocksr_decoder.h"
#include "../exception/unsupported_configuration.h"
#include "../utils.h"

YAML::Node ShadowsocksRDecoder::decode_config(std::string &content) {
    throw UnsupportedConfiguration("ShadowsocksR is not supported yet");

    // decoding
    Utils::replace(content, {
                           {"_", "/"},
                           {"-", "+"}
                   }
    );
    auto decoded_config = decode_base64(content);

    // parsing
    auto ssr = Utils::split(decoded_config, '/');
    auto main_config = Utils::split(ssr[0], ':');

    YAML::Node node(YAML::NodeType::Map);
    node["ip"] = main_config[0];
    node["port"] = main_config[1];
    node["protocol"] = main_config[2];
    node["method"] = main_config[3];
    node["obfuscate"] = main_config[4];
    node["password"] = decode_base64(main_config[5]);

    // additional parameters
    node["parameters"] = get_ssr_parameters(ssr[1]);

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
            parameters.emplace(map_content[0], value);
        } else {
            spdlog::warn("discard parameter {}", pair);
        }
    }

    return parameters;
}