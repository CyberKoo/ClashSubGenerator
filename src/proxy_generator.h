//
// Created by Kotarou on 2020/3/16.
//

#ifndef CLASHSUBGENERATOR_PROXY_GENERATOR_H
#define CLASHSUBGENERATOR_PROXY_GENERATOR_H

#include <regex>
#include <string>
#include <yaml-cpp/yaml.h>

class ProxyGenerator {
public:
    using node_vector = std::vector<YAML::Node>;

    ProxyGenerator();

    void load(const std::string &uri);

    void set_provider(const YAML::Node &_provider);

    void set_emoji_map(const YAML::Node &_emoji_map);

    void grouping(size_t group_min_size);

    void set_name_parser(const std::string &pattern);

    void set_exclude_amplified_node(bool flag);

    void set_grouping(bool flag);

    YAML::Node get_yaml(bool use_emoji);

private:
    struct NameAttribute {
        std::string location;
        int id;
        bool netflix;
        float amplification;
    };

    NameAttribute parse_name(const std::string &name);

    std::string name2emoji(const std::string &name);

    static void append_attributes(const NameAttribute &attribute, YAML::Node &node);

private:
    YAML::Node proxies;
    YAML::Node provider;
    YAML::Node emoji_map;
    std::regex name_parser;
    bool enable_grouping;
    bool exclude_amplified_node;
    std::map<std::string, node_vector> group_result;
};

#endif //CLASHSUBGENERATOR_PROXY_GENERATOR_H
