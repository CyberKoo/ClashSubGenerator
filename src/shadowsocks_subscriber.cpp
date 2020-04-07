//
// Created by Kotarou on 2020/3/19.
//

#include <spdlog/spdlog.h>
#include "httpclient.h"
#include "base64.h"
#include "utils.h"
#include "shadowsocks_subscriber.h"
#include "exception/unsupported_configuration.h"

void ShadowsocksSubscriber::load(const std::string &uri) {
    uri.size();
    throw UnsupportedConfiguration("Shadowsocks is not supported yet");
}

std::vector<std::string> decode_config(const std::string &config) {
    return std::vector<std::string>{config};
}