//
// Created by Kotarou on 2020/3/15.
//

#ifndef CLASHSUBGENERATOR_CLASH_GENERATOR_H
#define CLASHSUBGENERATOR_CLASH_GENERATOR_H

#include <yaml-cpp/node/node.h>
#include "config.h"

class ClashSubGenerator {
public:
    explicit ClashSubGenerator(const Config &_config) : config(_config) {};

    void run();

    static std::string version();

private:
    YAML::Node generate_configuration(const YAML::Node &node, const YAML::Node &preferred_group);

    YAML::Node get_config(std::string_view filename, std::string_view repository_filename);

    std::string get_file_full_path(std::string_view filename);

    YAML::Node create_emoji_map(std::string_view provider_name);

    YAML::Node generate_provider_configuration(const YAML::Node &node);

    YAML::Node regulate_groups(const YAML::Node &groups);

    YAML::Node create_proxy_group(const std::string &group_name, ProxyGroupType proxyGroupType);

    YAML::Node
    create_provider_group(ProviderType providerType, const std::string &path, const std::string &url, bool hc_enable);

    std::string get_provider_type_name(ProviderType providerType);

    std::string get_group_type_name(ProxyGroupType proxyGroupType);

private:
    const Config &config;

    YAML::Node system_config;
};

#endif //CLASHSUBGENERATOR_CLASH_GENERATOR_H
