//
// Created by Kotarou on 2020/8/7.
//

#ifndef CLASHSUBGENERATOR_CONFIG_LOADER_H
#define CLASHSUBGENERATOR_CONFIG_LOADER_H

#include <string>
#include <filesystem>
#include <yaml-cpp/node/node.h>

class Uri;

class ConfigLoader {
public:
    ~ConfigLoader() = default;

    static std::shared_ptr<ConfigLoader> instance();

    std::string load_raw(std::string_view uri, bool local_only = false, bool use_cache = false);

    YAML::Node load_yaml(std::string_view uri, bool local_only = false, bool use_cache = false);

    void destroy_cache();
private:
    ConfigLoader() = default;

    std::string cache_loader(const Uri &uri, bool use_cache);

    static void validate_schema(const Uri &uri);

private:
    std::map<std::string, std::string> cache;
};


#endif //CLASHSUBGENERATOR_CONFIG_LOADER_H
