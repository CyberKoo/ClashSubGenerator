//
// Created by Kotarou on 2020/6/26.
//

#include <algorithm>
#include <spdlog/spdlog.h>
#include <yaml-cpp/yaml.h>

#include "proxy_decoder.h"
#include "socks5_decoder.h"
#include "vmess_decoder.h"
#include "trojan_decoder.h"
#include "shadowsocks_decoder.h"
#include "shadowsocksr_decoder.h"
#include "../uri.h"
#include "../base64.h"
#include "../exception/unsupported_configuration.h"

YAML::Node ProxyDecoder::decode(std::string &content) {
    auto uri = Uri::Parse(content);
    auto decoder = get_decoder(uri.getProtocol());
    spdlog::debug("selected {} decoder for processing the raw data", uri.getProtocol());
    return decoder->decode_config(uri);
}

std::string ProxyDecoder::decode_base64(std::string_view data) {
    auto decoded = Base64::decode(data);
    return Base64::to_string(decoded);
}

std::unique_ptr<ProxyDecoder> ProxyDecoder::get_decoder(std::string_view protocol) {
    if (protocol == "vmess") {
        return std::make_unique<VmessDecoder>();
    } else if (protocol == "ss") {
        return std::make_unique<ShadowsocksDecoder>();
    } else if (protocol == "ssr") {
        return std::make_unique<ShadowsocksRDecoder>();
    } else if (protocol == "socks") {
        return std::make_unique<Socks5Decoder>();
    } else if (protocol == "trojan") {
        return std::make_unique<TrojanDecoder>();
    }

    throw UnsupportedConfiguration(fmt::format("Unable to determine the protocol", protocol));
}

std::pair<std::string, std::string> ProxyDecoder::strip_name(std::string_view content) {
    auto comment_start = content.rfind('#');
    if (comment_start != std::string::npos) {
        return std::make_pair(content.substr(comment_start + 1, content.size() - 1).data(),
                              std::string(content.substr(0, comment_start)));
    }

    return std::make_pair("", content.data());
}


