//
// Created by Kotarou on 2020/6/26.
//

#include <fmt/format.h>
#include <yaml-cpp/yaml.h>

#include "shadowsocks_decoder.h"
#include "../uri.h"
#include "../utils.h"
#include "../exception/config_parse_exception.h"

YAML::Node ShadowsocksDecoder::decode_config(const Uri &uri) {
    auto[name, raw_config] = strip_name(uri.getHost());
    auto decoded_config = decode_base64(raw_config);
    proxy = parse_config(decoded_config);
    proxy["type"] = std::string("ss");
    proxy["name"] = Utils::url_decode(name, true);
    if (name.empty()) {
        proxy["name"] = fmt::format("shadowsocks_{}", Utils::get_random_string(10));
    }

    return proxy;
}

std::map<std::string, std::string> ShadowsocksDecoder::parse_config(std::string_view config) {
    std::map<std::string, std::string> parse_result;
    std::string new_cfg = config.data();
    parse_result.insert({"port", find_extract(new_cfg, ":", true)});
    parse_result.insert({"host", find_extract(new_cfg, "@", true)});
    parse_result.insert({"password", find_extract(new_cfg, ":", false)});
    parse_result.insert({"cipher", new_cfg});

    return parse_result;
}

std::string ShadowsocksDecoder::find_extract(std::string &str, std::string_view delimiter, bool reverse) {
    std::string::size_type pos;
    if (!reverse) {
        pos = str.find(delimiter);
    } else {
        pos = str.rfind(delimiter);
    }

    if (pos == std::string::npos) {
        throw ConfigParseException(fmt::format("unable to find position of delimiter {}", delimiter));
    }

    auto result = str.substr(pos + 1, str.size());
    str.resize(pos);

    return result;
}
