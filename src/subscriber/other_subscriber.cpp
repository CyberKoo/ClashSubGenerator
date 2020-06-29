//
// Created by Kotarou on 2020/3/19.
//

#include <spdlog/spdlog.h>
#include <yaml-cpp/yaml.h>

#include "other_subscriber.h"
#include "../base64.h"
#include "../utils.h"
#include "../httpclient.h"
#include "../proxy_decoder/proxy_decoder.h"
#include "../exception/csg_exeption.h"

void OtherSubscriber::load(const std::string &uri) {
    auto config = HttpClient::get(uri);
    proxies = decode_config(config);

    if (proxies.size() > 0) {
        spdlog::info("Total number of proxies loaded {}", proxies.size());
    } else {
        spdlog::warn("No proxy loaded, this is probably not what you expected", proxies.size());
    }
}

YAML::Node OtherSubscriber::decode_config(const std::string &config) {
    auto decoded_config = Base64::decode(config);
    YAML::Node proxies;

    auto config_list = Utils::split(Base64::to_string(decoded_config), '\n');
    for (auto &proxy: config_list) {
        try {
            // decode config
            auto proxy_config = ProxyDecoder::decode(proxy);

            if (proxy_config.IsDefined()) {
                proxies.push_back(proxy_config);
            }
        } catch (CSGRuntimeException &e) {
            spdlog::warn("Skip adding proxy {}, due to {}", proxy, e.what());
        }
    }

    return proxies;
}
