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
    YAML::Node generate_config_file(const YAML::Node &node, const YAML::Node &preferred_group);

    YAML::Node get_config(std::string_view filename, std::string_view repository_filename);

    std::string get_file_full_path(std::string_view filename);

    YAML::Node create_emoji_map(std::string_view provider_name);

    YAML::Node generate_providers(const YAML::Node &node);

    YAML::Node build_groups(const YAML::Node &groups);

    YAML::Node yaml_proxy_group(const std::string &group_name, ProxyGroupType proxyGroupType);

    YAML::Node yaml_provider_group(ProviderType type, const std::string &path, const std::string &url, bool hc_enable);

    static std::string get_provider_type_name(ProviderType providerType);

    static std::string get_group_type_name(ProxyGroupType proxyGroupType);

private:
    const Config &config;

    YAML::Node system_config;

    static constexpr std::string_view ungrouped_name = "Ungrouped";
};

#endif //CLASHSUBGENERATOR_CLASH_GENERATOR_H
