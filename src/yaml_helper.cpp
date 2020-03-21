//
// Created by Kotarou on 2020/3/16.
//
#include <fmt/format.h>
#include <yaml-cpp/yaml.h>
#include <spdlog/spdlog.h>

#include "yaml_helper.h"
#include "httpclient.h"
#include "utils.h"
#include "exception/missing_key_exception.h"
#include "exception/file_not_exists_exception.h"

YAML::Node YAMLHelper::load_remote(const std::string &uri) {
    auto remote_config = HttpClient::get(uri);
    return YAML::Load(remote_config);
}

YAML::Node YAMLHelper::load_local(const std::string &path) {
    if (Utils::file_exists(path)) {
        return YAML::LoadFile(path);
    }

    throw FileNotExistsException(fmt::format("File {} doesn't exist", path));
}

std::string YAMLHelper::search_key(const YAML::Node &node, const std::vector<std::string> &keys) {
    for (auto &key :keys) {
        if (node[key].IsDefined()) {
            return key;
        }
    }

    throw MissingKeyException(fmt::format("Key {} is not found", keys.front()));
}

void YAMLHelper::write_yaml(const YAML::Node &node, const std::string &file) {
    std::ofstream fout(file);
    if (fout.is_open()) {
        spdlog::info("Writing yaml to file {}", file);
        fout << node;
        fout.close();
    }
}

void YAMLHelper::node_renamer(const YAML::Node &node, const std::map<std::string, std::string> &replace_pair) {
    for (auto item: node) {
        auto key_name = item.first.as<std::string>();
        if (replace_pair.count(key_name)) {
            item.first = replace_pair.at(key_name);
            spdlog::trace("Replace key {} to {}", key_name, replace_pair.at(key_name));
            continue;
        }
    }
}

void YAMLHelper::node_merger(const YAML::Node &source_node, YAML::Node target_node) {
    for (const auto &node : source_node) {
        target_node.push_back(YAML::Node(node));
    }
}

YAML::Node YAMLHelper::create_proxy_group(const std::string &group_name, const std::string &type,
                                          const std::string &url, int interval) {
    auto group_content = YAML::Node();
    group_content["name"] = YAML::Node(group_name);
    group_content["type"] = YAML::Node(type);
    group_content["url"] = YAML::Node(url);
    group_content["interval"] = YAML::Node(interval);
    group_content["proxies"] = YAML::Node(YAML::NodeType::Sequence);

    return group_content;
}
