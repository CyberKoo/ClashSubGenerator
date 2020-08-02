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

private:
    const Config &config;

    YAML::Node system_config;

};

#endif //CLASHSUBGENERATOR_CLASH_GENERATOR_H
