//
// Created by Kotarou on 2020/3/16.
//

#ifndef CLASHSUBGENERATOR_YAML_HELPER_H
#define CLASHSUBGENERATOR_YAML_HELPER_H

#include <map>
#include <yaml-cpp/node/node.h>

#include "enumerate.h"

namespace YAMLHelper {
    YAML::Node load_remote(std::string_view uri);

    YAML::Node load_local(const std::string &path);

    void write_yaml(const YAML::Node &node, std::string_view file);

    std::string search_key(const YAML::Node &node, const std::vector<std::string> &keys);

    void node_merger(const YAML::Node &source_node, YAML::Node target_node);

    void node_renamer(const YAML::Node &node, const std::map<std::string, std::string> &replace_pair);

    void node_renamer(const YAML::Node &node, std::string_view search, const std::string &replace);

    template<class T>
    void format(T &&node, YAML::EmitterStyle::value style, bool recursive = false) {
        node.SetStyle(style);

        if (recursive) {
            for (auto sub_node : node) {
                if (sub_node.IsDefined()) {
                    format(sub_node, style, true);
                } else {
                    if (sub_node.first.IsDefined()) {
                        format(sub_node.first, style, true);
                    }

                    if (sub_node.second.IsDefined()) {
                        format(sub_node.second, style, true);
                    }
                }
            }
        }
    }
}

#endif //CLASHSUBGENERATOR_YAML_HELPER_H
