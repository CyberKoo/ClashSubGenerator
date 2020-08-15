//
// Created by Kotarou on 2020/3/19.
//

#ifndef CLASHSUBGENERATOR_SUBSCRIBER_H
#define CLASHSUBGENERATOR_SUBSCRIBER_H

#include <map>
#include <regex>
#include <vector>
#include <yaml-cpp/node/node.h>

enum class SubscribeType;

class Subscriber {
public:
    using node_vector = std::vector<YAML::Node>;

    explicit Subscriber(SubscribeType type);

    ~Subscriber() = default;

    void load(std::string_view uri);

    void grouping(size_t group_min_size);

    YAML::Node get();

    void set_provider(const YAML::Node &_provider);

    void set_emoji_map(const YAML::Node &_emoji_map);

    void set_name_parser(const std::string &pattern);

    void set_exclude_amplified_node(bool flag);

    void set_grouping(bool flag);

    void set_regex_collapse(bool flag);

    void set_use_emoji(bool _use_emoji);

protected:
    // location, id, netflix, amplification
    using NameAttribute = std::tuple<std::string, int ,bool, float>;

    void clash_config_loader(std::string_view uri);

    void base64_config_loader(std::string_view uri);

    static YAML::Node decode_config(std::string_view config);

    std::vector<std::string> get_regex_result(const std::smatch &result) const;

    NameAttribute parse_name(const std::string &name);

    std::string name2emoji(const std::string &name);

    std::function<std::string(const YAML::Node &)> get_name_generator();

    void remove_groups(std::vector<std::string> &remove_list);

    static void append_attributes(const NameAttribute &attribute, YAML::Node &node);

protected:
    YAML::Node proxies;
    YAML::Node provider;
    YAML::Node emoji_map;
    std::regex name_parser;
    SubscribeType type;
    bool use_emoji;
    bool regex_collapse;
    bool enable_grouping;
    bool exclude_amplified_node;
    std::map<std::string, node_vector> group_result;
};

#endif //CLASHSUBGENERATOR_SUBSCRIBER_H
