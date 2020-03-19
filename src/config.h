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
    CONFIG,
    PROVIDER
};

enum class SubscribeType {
    CLASH,
    V2RAY,
    SS
};

struct Config {
    std::string subscribe_url;
    SubscribeType subscribe_type;
    std::string provider_name;
    std::string rules_uri;
    std::string output;
    std::string working_directory;
    std::string config_file;
    std::string template_file;
    std::string repository_url;
    bool enable_grouping;
    bool use_emoji;
    bool exclude_amplified_proxy;
    bool local_only;
    bool debug;
    size_t group_min_size;
    Generator generator;
    Syntax syntax;
};
#endif //CLASHSUBGENERATOR_CONFIG_H
