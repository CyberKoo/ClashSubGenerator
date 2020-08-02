//
// Created by Kotarou on 2020/3/16.
//

#include <spdlog/spdlog.h>
#include <yaml-cpp/yaml.h>

#include "clash_subscriber.h"
#include "../yaml_helper.h"
#include "../exception/missing_key_exception.h"

void ClashSubscriber::load(std::string_view uri) {
    try {
        auto yaml = YAMLHelper::load_remote(uri);
        proxies = yaml["proxies"];
        spdlog::info("Total number of proxies loaded {}", proxies.size());
    } catch (YAML::ParserException &e) {
        spdlog::critical("Invalid configuration file. {}", e.what());
        std::abort();
    } catch(MissingKeyException &e) {
        spdlog::critical("{}, the uri supplied does not contain proxy section or is not a valid clash configuration file. You may want to change the subscriber type to other", e.what());
        std::abort();
    }
}
