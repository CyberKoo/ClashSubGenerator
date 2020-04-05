//
// Created by Kotarou on 2020/3/16.
//

#include <spdlog/spdlog.h>

#include "clash_subscriber.h"
#include "yaml_helper.h"

void ClashSubscriber::load(const std::string &uri) {
    try {
        auto yaml = YAMLHelper::load_remote(uri);
        auto proxy_key_name = YAMLHelper::search_key(yaml, {"Proxy", "proxies"});
        proxies = yaml[proxy_key_name];
        spdlog::info("Total number of proxies loaded {}", proxies.size());
    } catch (YAML::ParserException &e) {
        spdlog::critical("Invalid configuration file. {}", e.what());
        std::abort();
    }
}
