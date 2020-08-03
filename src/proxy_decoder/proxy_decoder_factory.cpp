//
// Created by Kotarou on 2020/8/3.
//

#include <fmt/format.h>
#include <yaml-cpp/yaml.h>

#include "proxy_decoder_factory.h"
#include "vmess_decoder.h"
#include "socks5_decoder.h"
#include "trojan_decoder.h"
#include "shadowsocks_decoder.h"
#include "shadowsocksr_decoder.h"
#include "../exception/unsupported_configuration.h"

std::unique_ptr<ProxyDecoder> ProxyDecoderFactory::make(std::string_view protocol) {
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
