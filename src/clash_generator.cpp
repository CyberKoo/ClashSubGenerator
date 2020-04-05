//
// Created by Kotarou on 2020/3/15.
//
#include <version.h>
#include <algorithm>
#include <fmt/format.h>
#include <spdlog/spdlog.h>

#include "utils.h"
#include "yaml_helper.h"
#include "rule_extractor.h"
#include "clash_generator.h"
#include "clash_subscriber.h"
#include "v2ray_subscriber.h"
#include "shadowsocks_subscriber.h"
#include "exception/missing_key_exception.h"
#include "exception/file_not_exists_exception.h"
#include "exception/unsupported_configuration.h"

void ClashSubGenerator::run() {
    spdlog::info("Configuration syntax: {}",
                 config.syntax == Syntax::MODERN ? "Modern (requires Clash version > v0.18.0)" : "Legacy");
    spdlog::info("Configuration format: {}", config.generator == Generator::PROVIDER ? "Provider" : "Config");

    system_config = get_config(config.config_file, "sys_config.yaml");
    auto provider = system_config["Providers"][config.provider_name];
    auto subscriber = get_subscriber();

    // provider related
    if (!config.provider_name.empty()) {
        // validate provider
        if (system_config["Providers"][config.provider_name].IsDefined()) {
            subscriber->set_provider(provider);
            subscriber->set_grouping(config.enable_grouping);
            subscriber->set_emoji_map(create_emoji_map(config.provider_name));
            subscriber->set_name_parser(provider["regex"].as<std::string>());
            if (provider["regex_collapse"].IsDefined()) {
                subscriber->set_regex_collapse(provider["regex_collapse"].as<bool>());
            }
            spdlog::info("Provider is set to {}", config.provider_name);
        } else {
            spdlog::warn("Provider {} is not defined in the config file, name2emoji may not work properly.",
                         config.provider_name);
            if (config.enable_grouping) {
                spdlog::warn("Grouping is disabled due to no valid provider provided.");
            }
        }
    } else {
        // only display warning when name to emoji is enabled
        if (config.use_emoji) {
            spdlog::warn("Provider name is not set, name2emoji may not work properly.");
        }
        subscriber->set_emoji_map(system_config["Global"]["location2emoji"]);
    }

    subscriber->load(config.subscribe_url);
    subscriber->grouping(config.group_min_size);
    subscriber->set_exclude_amplified_node(config.exclude_amplified_proxy);
    auto proxies = subscriber->get_yaml(config.use_emoji);
    auto clash_config = generate_configuration(proxies, provider["preferred_group"]);
    auto proxy_list = get_all_proxies_name(clash_config);

    if (config.syntax == Syntax::LEGACY) {
        legacy_syntax_converter(clash_config);
    }

    YAMLHelper::write_yaml(clash_config, get_file_full_path(config.output));
}

std::string ClashSubGenerator::version() {
    return fmt::format("ClashSubGenerator {0}.{1}.{2}-{3}\nCompilation date: {4} {5}", CSG_MAJOR, CSG_MINOR,
                       CSG_PATCH, CSG_RELEASE_INFO, __DATE__, __TIME__);
}

std::string ClashSubGenerator::get_file_full_path(const std::string &filename) {
    return fmt::format("{}/{}", config.working_directory, filename);
}

YAML::Node ClashSubGenerator::create_emoji_map(const std::string &provider_name) {
    auto emoji = YAML::Node(system_config["Global"]["location2emoji"]);
    auto provider = system_config["Providers"][provider_name];
    if (provider["location2emoji"].IsDefined() && provider["location2emoji"].size() != 0) {

        for (const auto &local_emoji : provider["location2emoji"]) {
            auto emoji_name = local_emoji.first.as<std::string>();
            spdlog::trace("add {} to emoji list", emoji_name);
            emoji[emoji_name] = local_emoji.second.as<std::string>();
        }
    }

    return emoji;
}

YAML::Node ClashSubGenerator::get_config(const std::string &filename, const std::string &repository_filename) {
    auto path = get_file_full_path(filename);
    if (Utils::file_exists(path)) {
        return YAMLHelper::load_local(path);
    } else {
        if (!config.local_only) {
            spdlog::warn("Unable to load local file: {}, download from repository", repository_filename);
            auto uri = fmt::format("{}/{}", config.repository_url, repository_filename);
            return YAMLHelper::load_remote(uri);
        } else {
            spdlog::error("Local only enabled, not allowed to fetch configuration from repository");
            throw FileNotExistsException(fmt::format("file {} doesn't exist", filename));
        }
    }
}

YAML::Node ClashSubGenerator::generate_configuration(const YAML::Node &node, const YAML::Node &preferred_group) {
    spdlog::info("Start generating Clash configuration file");
    auto yaml_template = get_config(config.template_file, "template.yaml");
    RuleExtractor rule_extractor;
    rule_extractor.load(config.rules_uri);
    auto rules = rule_extractor.get();

    auto group_name = node["group_name"];
    if (preferred_group.IsDefined() && preferred_group.IsScalar()) {
        auto p_group_name = preferred_group.as<std::string>();
        spdlog::debug("Preferred group is set to {}, trying to find and move it to the front", p_group_name);
        auto node_name_list = node["group_name"].as<std::vector<std::string>>();
        for (auto &name: node_name_list) {
            auto result = name.find(p_group_name);
            if (result != std::string::npos) {
                std::swap(node_name_list.front(), name);
                spdlog::debug("Group {} is moved to the front", p_group_name);
                break;
            }
        }
        group_name = YAML::Node(node_name_list);
    }

    YAMLHelper::node_merger(node["proxies"], yaml_template["proxies"]);
    YAMLHelper::node_merger(rules, yaml_template["rules"]);
    YAMLHelper::node_merger(node["groups"], yaml_template["proxy-groups"]);

    bool anchor_replaced = false;
    std::string anchor = "__ANCHOR__";
    auto anchor_group_name = config.provider_name.empty() ? "Generated" : config.provider_name;
    for (auto group : yaml_template["proxy-groups"]) {
        auto name = group["name"].as<std::string>();
        if (name == anchor) {
            spdlog::debug("Replace anchor with generated group {}", anchor_group_name);
            group["name"] = anchor_group_name;
            group["proxies"] = group_name;
            anchor_replaced = true;
            continue;
        }

        if (group["proxies"].IsDefined()) {
            auto proxies = group["proxies"].as<std::vector<std::string>>();
            if (std::find(proxies.begin(), proxies.end(), anchor) != proxies.end()) {
                spdlog::debug("Replace anchor proxy with provider name in group {}", name);
                std::replace(proxies.begin(), proxies.end(), anchor, anchor_group_name);
                group["proxies"] = proxies;
                continue;
            }
        }
    }

    // insert node when no anchor defined
    if (!anchor_replaced) {
        spdlog::debug("Anchor group not found, insert generated group to the end");
        auto group_node = YAMLHelper::create_proxy_group(anchor_group_name);
        group_node["proxies"] = group_name;
        yaml_template["proxy-groups"].push_back(group_node);
    }

    return yaml_template;
}

void ClashSubGenerator::legacy_syntax_converter(const YAML::Node &node) {
    YAMLHelper::node_renamer(node, {
            {"proxies",         "Proxy"},
            {"proxy-groups",    "Proxy Group"},
            {"rules",           "Rule"},
            {"proxy-providers", "proxy-provider"}
    });
}

std::vector<std::string> ClashSubGenerator::get_all_proxies_name(const YAML::Node &node) {
    std::vector<std::string> name_list;
    for (const auto &proxy: node["proxies"]) {
        name_list.emplace_back(proxy["name"].as<std::string>());
    }

    for (const auto &proxy: node["proxy-groups"]) {
        name_list.emplace_back(proxy["name"].as<std::string>());
    }

    return name_list;
}

std::unique_ptr<Subscriber> ClashSubGenerator::get_subscriber() {
    switch (config.subscribe_type) {
        case SubscribeType::CLASH:
            return std::make_unique<ClashSubscriber>();
        case SubscribeType::V2RAY:
            return std::make_unique<V2raySubscriber>();
        case SubscribeType::SS:
            return std::make_unique<ShadowsocksSubscriber>();
    }

    throw UnsupportedConfiguration(fmt::format("Unsupported subscribe type {}", config.subscribe_type));
}
