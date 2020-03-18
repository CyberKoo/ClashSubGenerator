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

    void write_yaml(const YAML::Node &node, const std::string &file);

    std::string search_key(const YAML::Node &node, const std::vector<std::string> &keys);

    void node_merger(const YAML::Node &source_node, YAML::Node target_node);

    void node_renamer(const YAML::Node &node, const std::map<std::string, std::string> &replace_pair);
}

#endif //CLASHSUBGENERATOR_YAML_HELPER_H
