//
// Created by Kotarou on 2020/3/19.
//

#ifndef CLASHSUBGENERATOR_SUBSCRIBER_H
#define CLASHSUBGENERATOR_SUBSCRIBER_H

#include <regex>
#include <vector>
#include <yaml-cpp/yaml.h>

class Subscriber {
public:
    using node_vector = std::vector<YAML::Node>;

    explicit Subscriber();

    virtual ~Subscriber() = 0;

    virtual void load(const std::string &uri) = 0;

    virtual void grouping(size_t group_min_size);

    virtual YAML::Node get_yaml(bool use_emoji);

    virtual void set_provider(const YAML::Node &_provider);

    virtual void set_emoji_map(const YAML::Node &_emoji_map);

    virtual void set_name_parser(const std::string &pattern);

    virtual void set_exclude_amplified_node(bool flag);

    virtual void set_grouping(bool flag);

protected:
    struct NameAttribute {
        std::string location;
        int id;
        bool netflix;
        float amplification;
    };

    NameAttribute parse_name(const std::string &name);

    std::string name2emoji(const std::string &name);

    static void append_attributes(const NameAttribute &attribute, YAML::Node &node);

protected:
    YAML::Node proxies;
    YAML::Node provider;
    YAML::Node emoji_map;
    std::regex name_parser;
    bool enable_grouping;
    bool exclude_amplified_node;
    std::map<std::string, node_vector> group_result;
};

#endif //CLASHSUBGENERATOR_SUBSCRIBER_H
