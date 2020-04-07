//
// Created by Kotarou on 2020/3/15.
//

#ifndef CLASHSUBGENERATOR_CONFIG_H
#define CLASHSUBGENERATOR_CONFIG_H

#include <string>

enum class Syntax {
    LEGACY, MODERN
};

enum class Generator {
    CONFIG, PROVIDER
};

enum class SubscribeType {
    CLASH, V2RAY, SS, SSR
};

struct Config {
    bool debug;
    bool use_emoji;
    bool local_only;
    bool enable_grouping;
    bool exclude_amplified_proxy;

    std::string output;
    std::string rules_uri;
    std::string subscribe_url;
    std::string provider_name;
    std::string config_file;
    std::string template_file;
    std::string repository_url;
    std::string working_directory;

    size_t group_min_size;
    SubscribeType subscribe_type;
    Generator generator;
    Syntax syntax;
};
#endif //CLASHSUBGENERATOR_CONFIG_H
