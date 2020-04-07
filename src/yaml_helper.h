//
// Created by Kotarou on 2020/3/16.
//

#ifndef CLASHSUBGENERATOR_YAML_HELPER_H
#define CLASHSUBGENERATOR_YAML_HELPER_H

#include <map>
#include <yaml-cpp/node/node.h>

namespace YAMLHelper {
    YAML::Node load_remote(const std::string &uri);

    YAML::Node load_local(const std::string &path);

    YAML::Node create_proxy_group(const std::string &group_name, const std::string &type = "select",
                                  const std::string &url = "https://www.bing.com", int interval = 120);

    YAML::Node create_provider_group(const std::string &type = "file", const std::string &path = "",
                                     const std::string &url = "", bool hc_enable = true,
                                     const std::string &hc_url = "https://www.bing.com", int hc_interval = 120);

    void write_yaml(const YAML::Node &node, const std::string &file);

    std::string search_key(const YAML::Node &node, const std::vector<std::string> &keys);

    void node_merger(const YAML::Node &source_node, YAML::Node target_node);

    void node_renamer(const YAML::Node &node, const std::map<std::string, std::string> &replace_pair);

    void node_renamer(const YAML::Node &node, const std::string &search, const std::string &replace);
}

#endif //CLASHSUBGENERATOR_YAML_HELPER_H
