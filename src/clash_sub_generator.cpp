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
#include "proxy_generator.h"
#include "clash_sub_generator.h"
#include "exception/missing_key_exception.h"
#include "exception/file_not_exists_exception.h"

void ClashSubGenerator::run() {
    spdlog::info("Configuration syntax: {}",
                 config.syntax == Syntax::MODERN ? "Modern (requires Clash version > v0.18.0)" : "Legacy");
    spdlog::info("Configuration format: {}", config.generator == Generator::PROVIDER ? "Provider" : "Config");

    system_config = get_config(config.config_file, "sys_config.yaml");
    ProxyGenerator generator;

    // provider related
    if (!config.provider_name.empty()) {
        // validate provider
        auto provider = system_config["Providers"][config.provider_name];
        if (system_config["Providers"][config.provider_name].IsDefined()) {
            generator.set_provider(provider);
            generator.set_grouping(config.enable_grouping);
            generator.set_emoji_map(create_emoji_map(config.provider_name));
            generator.set_name_parser(provider["regex"].as<std::string>());
            spdlog::info("Provider is set to {}", config.provider_name);
        } else {
            spdlog::warn("Provider {} is not defined in the config file, name2emoji may not work properly.",
                         config.provider_name);
            if (config.enable_grouping) {
                spdlog::warn("Grouping is disabled due to no valid provider provided.");
            }
        }
    } else {
        spdlog::warn("Provider name is not set, name2emoji may not work properly.");
        generator.set_emoji_map(system_config["Global"]["location2emoji"]);
    }

    generator.load(config.subscribe_url);
    generator.grouping(config.group_min_size);
    generator.set_exclude_amplified_node(config.exclude_amplified_proxy);
    auto proxies = generator.get_yaml(config.use_emoji);
    auto clash_config = generate_configuration(proxies);
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
            spdlog::trace("增加 {} 到emoji列表中", emoji_name);
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

YAML::Node ClashSubGenerator::generate_configuration(const YAML::Node &node) {
    auto yaml_template = get_config(config.template_file, "template.yaml");
    RuleExtractor rule_extractor;
    rule_extractor.load(config.rules_uri);
    auto rules = rule_extractor.get();

    YAMLHelper::node_merger(node["proxies"], yaml_template["proxies"]);
    YAMLHelper::node_merger(rules, yaml_template["rules"]);
    YAMLHelper::node_merger(node["groups"], yaml_template["proxy-groups"]);

    bool anchor_replaced = false;
    std::string anchor = "__ANCHOR__";
    auto proxy_name = config.provider_name.empty() ? "Generated" : config.provider_name;
    for (auto group : yaml_template["proxy-groups"]) {
        if (group["name"].as<std::string>() == anchor) {
            group["name"] = proxy_name;
            group["proxies"] = node["group_name"];
            anchor_replaced = true;
            continue;
        }

        if (group["proxies"].IsDefined()) {
            auto proxies = group["proxies"].as<std::vector<std::string>>();
            if (std::find(proxies.begin(), proxies.end(), anchor) != proxies.end()) {
                std::replace(proxies.begin(), proxies.end(), anchor, proxy_name);
                group["proxies"] = proxies;
                continue;
            }
        }
    }

    // insert node when no anchor defined
    if (!anchor_replaced) {
        auto group_node = YAML::Node();
        group_node["name"] = YAML::Node(proxy_name);
        group_node["type"] = YAML::Node("select");
        group_node["proxies"] = node["group_name"];
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
