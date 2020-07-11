//
// Created by Kotarou on 2020/3/19.
//

#ifndef CLASHSUBGENERATOR_SUBSCRIBER_H
#define CLASHSUBGENERATOR_SUBSCRIBER_H

#include <regex>
#include <vector>
#include <yaml-cpp/node/node.h>

class Subscriber {
public:
    using node_vector = std::vector<YAML::Node>;

    explicit Subscriber();

    virtual ~Subscriber() = 0;

    virtual void load(const std::string &uri) = 0;

    virtual void grouping(size_t group_min_size);

    virtual YAML::Node get();

    void set_provider(const YAML::Node &_provider);

    void set_emoji_map(const YAML::Node &_emoji_map);

    void set_name_parser(const std::string &pattern);

    void set_exclude_amplified_node(bool flag);

    void set_grouping(bool flag);

    void set_regex_collapse(bool flag);

    void set_use_emoji(bool _use_emoji);

    void set_benchmarking_url(const std::string &name);

protected:
    struct NameAttribute {
        std::string location;
        int id;
        bool netflix;
        float amplification;
    };

    std::vector<std::string> get_regex_result(const std::smatch &result);

    NameAttribute parse_name(const std::string &name);

    std::string name2emoji(const std::string &name);

    static void append_attributes(const NameAttribute &attribute, YAML::Node &node);

protected:
    YAML::Node proxies;
    YAML::Node provider;
    YAML::Node emoji_map;
    std::regex name_parser;
    std::string benchmarking_url;
    bool use_emoji;
    bool regex_collapse;
    bool enable_grouping;
    bool exclude_amplified_node;
    std::map<std::string, node_vector> group_result;
};

#endif //CLASHSUBGENERATOR_SUBSCRIBER_H
