//
// Created by Kotarou on 2020/3/19.
//

#include <spdlog/spdlog.h>
#include <yaml-cpp/yaml.h>

#include "other_subscriber.h"
#include "../uri.h"
#include "../base64.h"
#include "../utils.h"
#include "../config_loader.h"
#include "../exception/csg_exeption.h"
#include "../proxy_decoder/proxy_decoder.h"
#include "../proxy_decoder/proxy_decoder_factory.h"

void OtherSubscriber::load(std::string_view uri) {
    auto config = ConfigLoader::load_raw(uri);
    proxies = decode_config(config);

    if (proxies.size() > 0) {
        spdlog::info("Total number of proxies loaded {}", proxies.size());
    } else {
        spdlog::warn("No proxy loaded, this is probably not what you expected", proxies.size());
    }
}

YAML::Node OtherSubscriber::decode_config(std::string_view config) {
    auto decoded_config = Base64::decode(config);
    YAML::Node proxies;

    auto config_list = Utils::split(decoded_config, '\n');
    for (auto &proxy: config_list) {
        try {
            if (!proxy.empty()) {
                Utils::trim(proxy);
                // decode config
                auto uri = Uri::Parse(proxy);
                auto decoder = ProxyDecoderFactory::make(uri.getSchema());
                spdlog::debug("select {} decoder for processing the raw data", uri.getSchema());

                auto proxy_config = decoder->decode_config(uri);
                if (proxy_config.IsDefined()) {
                    proxies.push_back(proxy_config);
                }
            }
        } catch (CSGRuntimeException &e) {
            spdlog::warn("Skip adding proxy {}, due to {}", proxy, e.what());
        }
    }

    return proxies;
}
