//
// Created by Kotarou on 2020/6/26.
//

#include <algorithm>
#include <spdlog/spdlog.h>
#include <yaml-cpp/yaml.h>

#include "proxy_decoder.h"
#include "socks5_decoder.h"
#include "vmess_decoder.h"
#include "shadowsocks_decoder.h"
#include "shadowsocksr_decoder.h"
#include "../base64.h"
#include "../utils.h"
#include "../exception/unsupported_configuration.h"

YAML::Node ProxyDecoder::decode(std::string &content) {
    auto result = strip(content);
    Utils::trim(result.second);
    auto decoder = get_decoder(result.first);
    return decoder->decode_config(result.second);
}

std::string ProxyDecoder::decode_base64(std::string &data) {
    auto decoded = Base64::decode(data);
    return Base64::to_string(decoded);
}

std::pair<std::string, std::string> ProxyDecoder::strip(std::string &uri) {
    auto pos = uri.find("://");

    if (pos != std::string::npos) {
        return std::make_pair(uri.substr(0, pos), uri.substr(pos + 3));
    }

    throw UnsupportedConfiguration(fmt::format("Unable to determine the protocol", uri));
}

std::unique_ptr<ProxyDecoder> ProxyDecoder::get_decoder(const std::string &protocol) {
    if (protocol == "vmess") {
        return std::make_unique<VmessDecoder>();
    } else if (protocol == "ss") {
        return std::make_unique<ShadowsocksDecoder>();
    } else if (protocol == "ssr") {
        return std::make_unique<ShadowsocksRDecoder>();
    } else if (protocol == "socks") {
        return std::make_unique<Socks5Decoder>();
    }

    throw UnsupportedConfiguration(fmt::format("Unable to determine the protocol", protocol));
}

std::pair<std::string, std::string> ProxyDecoder::strip_name(const std::string &content) {
    auto comment_start = content.rfind('#');
    if (comment_start != std::string::npos) {
        return std::make_pair(content.substr(comment_start + 1, content.size() - 1), content.substr(0, comment_start));
    }

    return std::make_pair("", content);
}


