//
// Created by Kotarou on 2020/3/16.
//

#include <spdlog/spdlog.h>

#include "proxy_generator.h"
#include "yaml_helper.h"

ProxyGenerator::ProxyGenerator() {
    this->enable_grouping = false;
    this->exclude_amplified_node = false;
}

void ProxyGenerator::load(const std::string &uri) {
    auto yaml = YAMLHelper::load_remote(uri);
    auto proxy_key_name = YAMLHelper::search_key(yaml, {"Proxy", "proxies"});
    proxies = yaml[proxy_key_name];
    spdlog::info("Total number of proxies loaded {}", proxies.size());
}

void ProxyGenerator::grouping(size_t group_min_size) {
    auto netflix = node_vector();
    auto leftover = node_vector();

    // cleaner
    std::vector<std::string> remove_list;
    const auto map_cleaner = [&]() {
        for (const auto &name: remove_list) {
            group_result.erase(name);
            spdlog::debug("Remove group {}", name);
        }
        remove_list.clear();
    };

    if (enable_grouping) {
        spdlog::info("Grouping proxies by name, minimum size for a group is {}", group_min_size);
        // grouping
        for (auto proxy : proxies) {
            auto proxy_name = proxy["name"].as<std::string>();
            auto attribute = parse_name(proxy_name);
            spdlog::trace("proxy name: {}, id: {}, netflix: {}, amplification: {}",
                          attribute.location, attribute.id, attribute.netflix, attribute.amplification);

            // write attributes to proxy
            append_attributes(attribute, proxy);

            if (attribute.amplification > 1.0) {
                spdlog::debug("Proxy {} excluded, because the amplification is {}", proxy_name,
                              attribute.amplification);
                continue;
            }

            // initialize vector
            if (!group_result.count(attribute.location)) {
                group_result.insert({attribute.location, node_vector()});
            }

            // insert into map
            group_result.at(attribute.location).push_back(proxy);

            // insert into netflix map
            if (attribute.netflix) {
                netflix.push_back(proxy);
            }
        }

        // post processing
        for (auto const &merge_pair: provider["group_merge"]) {
            auto source_name = merge_pair.first.as<std::string>();
            auto target_name = merge_pair.second.as<std::string>();
            if (group_result.count(source_name) && group_result.count(target_name)) {
                spdlog::debug("Move all proxies in group {} to {}", source_name, target_name);
                auto &source_vec = group_result.at(source_name);
                auto &target_vec = group_result.at(target_name);

                target_vec.insert(target_vec.end(), source_vec.begin(), source_vec.end());
                remove_list.emplace_back(source_name);
            }
        }
        map_cleaner();

        // move group size > group_min_size to left over
        for (const auto &node: group_result) {
            if (node.second.size() < group_min_size) {
                leftover.insert(leftover.end(), node.second.begin(), node.second.end());
                remove_list.emplace_back(node.first);
            }
        }
        map_cleaner();
    } else {
        spdlog::info("Grouping proxy is disabled");
        for (const auto &proxy : proxies) {
            leftover.emplace_back(proxy);
        }
    }

    if (!netflix.empty()) {
        spdlog::debug("Found {} netflix proxies", netflix.size());
        group_result.insert({"netflix", netflix});
    }

    if (!leftover.empty()) {
        spdlog::debug("Found {} leftover proxies", leftover.size());
        group_result.insert({"leftover", leftover});
    }
}

std::string ProxyGenerator::name2emoji(const std::string &name) {
    if (emoji_map[name].IsDefined()) {
        return emoji_map[name].as<std::string>();
    }

    spdlog::info("No emoji defined for {}", name);

    return name;
}

ProxyGenerator::NameAttribute ProxyGenerator::parse_name(const std::string &name) {
    NameAttribute attribute{};
    std::smatch match;
    if (std::regex_match(name, match, name_parser)) {
        spdlog::trace("Name {}, total number of matches are {}", name, match.size());

        auto get_definition = [&](const std::string &key_name) {
            return provider["definition"][key_name].as<int>();
        };

        // do have mapper
        if (provider["definition"].IsDefined()) {
            auto amp = match[get_definition("amplification")].str();
            auto id_def = get_definition("position");
            attribute.location = match[get_definition("location_name")].str();
            attribute.id = (id_def != -1) ? std::stoi(match[id_def].str()) : -1;
            attribute.netflix = !match[get_definition("netflix")].str().empty();
            attribute.amplification = amp.empty() ? 1.0f : std::stof(amp);
        }
    }

    return attribute;
}

YAML::Node ProxyGenerator::get_yaml(bool use_emoji) {
    auto node = YAML::Node();

    node["groups"] = YAML::Node(YAML::NodeType::Sequence);
    node["proxies"] = YAML::Node(YAML::NodeType::Sequence);
    node["group_name"] = YAML::Node(YAML::NodeType::Sequence);
    if (!group_result.empty()) {
        auto prefix = provider["prefix"].IsDefined() ? provider["prefix"].as<std::string>() : "Generated";
        for (const auto &group: group_result) {
            auto group_name = fmt::format("{}-{}", prefix, group.first);
            node["group_name"].push_back(group_name);
            spdlog::debug("Processing group {}", group_name);

            auto group_content = YAML::Node();
            group_content["name"] = YAML::Node(group_name);
            group_content["type"] = YAML::Node("select");
            group_content["proxies"] = YAML::Node(YAML::NodeType::Sequence);
            node["groups"].push_back(group_content);

            size_t counter = 1;
            for (const auto &proxy: group.second) {
                const auto name_generator = [&](const YAML::Node &attributes) {
                    auto id = attributes["id"].as<int>();
                    auto name = attributes["location"].as<std::string>();
                    if (id == -1) {
                        id = counter++;
                    }

                    if (use_emoji) {
                        name = name2emoji(name);
                    }

                    return fmt::format("{}{:>02d}", name, id);
                };

                if (proxy.IsDefined() && proxy.IsMap()) {
                    auto local_proxy = YAML::Clone(proxy);
                    std::string proxy_name = local_proxy["name"].as<std::string>();
                    spdlog::trace("Add proxy {} to group {}", proxy_name, group_name);

                    // only update name when grouping is enabled
                    if (local_proxy["attributes"].IsDefined()) {
                        proxy_name = name_generator(local_proxy["attributes"]);
                        local_proxy["name"] = proxy_name;
                    }

                    // do not append duplicated proxy
                    if (group.first != "netflix") {
                        node["proxies"].push_back(local_proxy);
                    }
                    group_content["proxies"].push_back(proxy_name);
                    // strip attributes
                    local_proxy.remove("attributes");
                }
            }
        }
    }

    return node;
}

void ProxyGenerator::append_attributes(const ProxyGenerator::NameAttribute &attribute, YAML::Node &node) {
    node["attributes"] = YAML::Node(YAML::NodeType::Map);
    node["attributes"].force_insert("location", attribute.location);
    node["attributes"].force_insert("id", attribute.id);
    node["attributes"].force_insert("netflix", attribute.netflix);
    node["attributes"].force_insert("amplification", attribute.amplification);
}

void ProxyGenerator::set_grouping(bool flag) {
    this->enable_grouping = flag;
}

void ProxyGenerator::set_provider(const YAML::Node &_provider) {
    this->provider = _provider;
}

void ProxyGenerator::set_emoji_map(const YAML::Node &_emoji_map) {
    this->emoji_map = _emoji_map;
}

void ProxyGenerator::set_exclude_amplified_node(bool flag) {
    this->exclude_amplified_node = flag;
}

void ProxyGenerator::set_name_parser(const std::string &pattern) {
    this->name_parser = std::regex(pattern, std::regex_constants::icase);
}
