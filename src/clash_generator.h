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

    YAML::Node get_config(const std::string &filename, const std::string &repository_filename);

    std::string get_file_full_path(const std::string &filename);

    YAML::Node create_emoji_map(const std::string &provider_name);

    YAML::Node generate_provider_configuration(const YAML::Node &node);

    static void legacy_syntax_converter(const YAML::Node &node);

    static std::vector<std::string> get_all_proxies_name(const YAML::Node &node);

private:
    const Config &config;

    YAML::Node system_config;

};

#endif //CLASHSUBGENERATOR_CLASH_GENERATOR_H
