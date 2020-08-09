//
// Created by Kotarou on 2020/3/15.
//
#include <version.h>
#include <algorithm>
#include <fmt/format.h>
#include <spdlog/spdlog.h>
#include <yaml-cpp/yaml.h>

#include "uri.h"
#include "hash.h"
#include "utils.h"
#include "filesystem.h"
#include "yaml_helper.h"
#include "config_loader.h"
#include "rule_extractor.h"
#include "clash_generator.h"
#include "subscriber/subscriber.h"
#include "subscriber/subscriber_factory.h"
#include "exception/file_system_exception.h"
#include "exception/missing_key_exception.h"
#include "exception/invalid_value_exception.h"

void ClashSubGenerator::run() {
    spdlog::info("Configuration format: {}", config.generator == Generator::PROVIDER ? "Provider" : "Config");

    system_config = get_config(config.config_file, "sys_config.yaml");
    auto provider = system_config["Providers"][config.provider_name];
    auto subscriber = SubscriberFactory::make(config.subscribe_type);

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

    // load yaml file from remote server
    subscriber->load(config.subscribe_url);
    // set flag and parameters
    subscriber->set_use_emoji(config.use_emoji);
    subscriber->set_exclude_amplified_node(config.exclude_amplified_proxy);
    // perform grouping
    subscriber->grouping(config.group_min_size);
    auto proxies = subscriber->get();
    if (config.generator == Generator::PROVIDER) {
        proxies = generate_providers(proxies);
    }

    auto clash_config = generate_config_file(proxies, provider["preferred_group"]);

    // format configurations
    constexpr char keys[][16] = {"proxies", "proxy-groups", "rules", "proxy-providers"};
    for (const auto &key : keys) {
        if (clash_config[key].IsDefined()) {
            YAMLHelper::format(clash_config[key], YAML::EmitterStyle::Block, true);
        }
    }

    // write config.yaml
    YAMLHelper::write_yaml(clash_config, get_file_full_path(config.output));
}

YAML::Node ClashSubGenerator::create_emoji_map(std::string_view provider_name) {
    auto emoji = YAML::Node(system_config["Global"]["location2emoji"]);
    auto provider = system_config["Providers"][provider_name.data()];
    if (provider["location2emoji"].IsDefined() && provider["location2emoji"].size() != 0) {
        for (const auto &local_emoji : provider["location2emoji"]) {
            auto emoji_name = local_emoji.first.as<std::string>();
            spdlog::trace("add {} to emoji list", emoji_name);
            emoji[emoji_name] = local_emoji.second.as<std::string>();
        }
    }

    return emoji;
}

YAML::Node ClashSubGenerator::get_config(std::string_view filename, std::string_view repository_filename) {
    auto path = get_file_full_path(filename);
    if (FileSystem::exists(path)) {
        return ConfigLoader::load_local_yaml(path);
    } else {
        if (!config.local_only) {
            spdlog::warn("Unable to load local file: {}, download from repository", repository_filename);
            auto uri = Uri::Parse(fmt::format("{}/{}", config.repository_url, repository_filename));
            return ConfigLoader::load_remote_yaml(uri);
        } else {
            spdlog::error("Local only enabled, not allowed to fetch configuration from repository");
            throw FileSystemException(fmt::format("file {} doesn't exist", filename));
        }
    }
}

YAML::Node ClashSubGenerator::generate_config_file(const YAML::Node &node, const YAML::Node &preferred_group) {
    spdlog::info("Start generating Clash configuration file");
    auto yaml_template = get_config(config.template_file, "template.yaml");

    auto group_name = node["group_name"];
    if (preferred_group.IsDefined() && preferred_group.IsScalar()) {
        auto p_group_name = preferred_group.as<std::string>();
        spdlog::debug("Preferred group is set to {}, trying to find and move it to the front", p_group_name);
        auto node_name_list = node["group_name"].as<std::vector<std::string>>();

        for (auto &name: node_name_list) {
            if (name.find(p_group_name) != std::string::npos) {
                std::swap(node_name_list.front(), name);
                spdlog::debug("Group {} is moved to the front", p_group_name);
                break;
            }
        }
        group_name = YAML::Node(node_name_list);
    }

    auto proxy_groups = node["groups"];
    if (config.generator == Generator::CONFIG) {
        proxy_groups = build_groups(proxy_groups);
    }
    YAMLHelper::node_merger(node["proxies"], yaml_template["proxies"]);
    YAMLHelper::node_merger(proxy_groups, yaml_template["proxy-groups"]);

    // append proxy-providers
    if (node["proxy-providers"].IsDefined() && node["proxy-providers"].IsMap()) {
        yaml_template["proxy-providers"] = node["proxy-providers"];
    }

    bool anchor_replaced = false;
    const std::string anchor = "__ANCHOR__";
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
        auto group_node = yaml_proxy_group(anchor_group_name, ProxyGroupType::SELECT);
        group_node["proxies"] = group_name;
        yaml_template["proxy-groups"].push_back(group_node);
    }

    // replace anchor in user-defined rules
    for (const auto &rule : yaml_template["rules"]) {
        auto s_rule = rule.as<std::string>();
        if (s_rule.find(anchor) != std::string::npos) {
            Utils::replace(s_rule, {{anchor, anchor_group_name}});
            (YAML::Node(rule)) = s_rule;
        }
    }

    // merge external rule, if applied
    if (!config.rules_uri.empty()) {
        spdlog::info("External rule {} will be merged", config.rules_uri);
        RuleExtractor rule_extractor;
        rule_extractor.load(config.rules_uri);

        YAMLHelper::node_merger(rule_extractor.get(), yaml_template["rules"]);
    }

    return yaml_template;
}

YAML::Node ClashSubGenerator::generate_providers(const YAML::Node &node) {
    const auto directory_name = get_file_full_path("providers");
    // create directory if not exists
    if (!FileSystem::exists(directory_name)) {
        spdlog::debug("directory {} not exist, creating...", directory_name);
        FileSystem::mkdir(directory_name);
    } else {
        spdlog::debug("clearing directory {}", directory_name);
        FileSystem::clear_directory(directory_name);
    }

    auto master_config = YAML::Node();
    master_config["proxy-providers"] = YAML::Node(YAML::NodeType::Map);
    master_config["groups"] = YAML::Node(YAML::NodeType::Sequence);
    master_config["group_name"] = node["group_name"];
    for (const auto &group: node["groups"]) {
        auto proxies_list = group["proxies"].as<std::vector<std::string>>();
        auto provider_proxies = YAML::Node();
        provider_proxies["proxies"] = YAML::Node(YAML::NodeType::Sequence);

        for (const auto &proxy_node : node["proxies"]) {
            const auto name = proxy_node["name"].as<std::string>();
            if (std::find(proxies_list.begin(), proxies_list.end(), name) != proxies_list.end()) {
                provider_proxies["proxies"].push_back(proxy_node);
            }
        }

        // write to file
        const auto group_name = group["name"].as<std::string>();
        auto out_file = fmt::format("providers/{}.yaml", Hash::md5(group_name));
        YAMLHelper::write_yaml(provider_proxies, get_file_full_path(out_file));

        // write provider section
        master_config["proxy-providers"][group_name] = yaml_provider_group(ProviderType::FILE, out_file,
                                                                           "", true);

        // write proxy groups
        auto search_result = group_name.find(ungrouped_name) == std::string::npos;
        auto proxy_group_type = search_result ? ProxyGroupType::URL_TEST : ProxyGroupType::SELECT;
        auto proxy_group = yaml_proxy_group(group_name, proxy_group_type);
        YAMLHelper::node_renamer(proxy_group, "proxies", "use");
        proxy_group["use"].push_back(group_name);
        master_config["groups"].push_back(proxy_group);
    }

    return master_config;
}

YAML::Node ClashSubGenerator::build_groups(const YAML::Node &groups) {
    YAML::Node new_groups = YAML::Node(YAML::NodeType::Sequence);
    for (const YAML::Node &group: groups) {
        auto group_name = group["name"].as<std::string>();
        auto search_result = group_name.find(ungrouped_name) == std::string::npos;
        auto proxy_group_type = search_result ? ProxyGroupType::URL_TEST : ProxyGroupType::SELECT;
        auto new_group = yaml_proxy_group(group_name, proxy_group_type);
        new_group["proxies"] = group["proxies"];

        new_groups.push_back(new_group);
    }

    return new_groups;
}

std::string ClashSubGenerator::version() {
    return fmt::format("{}\nCompilation date: {} {}", get_version(), __DATE__, __TIME__);
}

std::string ClashSubGenerator::get_file_full_path(std::string_view filename) {
    return fmt::format("{}{}", config.working_directory, filename);
}

YAML::Node ClashSubGenerator::yaml_proxy_group(const std::string &group_name, ProxyGroupType proxyGroupType) {
    auto group_content = YAML::Node();
    group_content["name"] = YAML::Node(group_name);
    group_content["type"] = YAML::Node(get_group_type_name(proxyGroupType));
    group_content["url"] = YAML::Node(config.benchmarking_url);
    group_content["interval"] = YAML::Node(config.benchmarking_interval);

    if (proxyGroupType == ProxyGroupType::URL_TEST && config.benchmarking_tolerance > 0) {
        group_content["tolerance"] = YAML::Node(config.benchmarking_tolerance);
    }

    group_content["proxies"] = YAML::Node(YAML::NodeType::Sequence);

    return group_content;
}

YAML::Node ClashSubGenerator::yaml_provider_group(ProviderType type, const std::string &path, const std::string &url,
                                                  bool hc_enable) {
    auto group_content = YAML::Node();
    group_content["type"] = YAML::Node(get_provider_type_name(type));

    if (type == ProviderType::HTTP) {
        if (!url.empty()) {
            group_content["url"] = YAML::Node(url);
        } else {
            throw MissingKeyException("Provider type http must be used with a valid url");
        }
    }

    group_content["path"] = YAML::Node(path);
    group_content["health-check"] = YAML::Node(YAML::NodeType::Map);
    group_content["health-check"]["enable"] = YAML::Node(hc_enable);
    group_content["health-check"]["url"] = YAML::Node(config.benchmarking_url);
    group_content["health-check"]["interval"] = YAML::Node(config.benchmarking_interval);

    return group_content;
}

std::string ClashSubGenerator::get_group_type_name(ProxyGroupType proxyGroupType) {
    switch (proxyGroupType) {
        case ProxyGroupType::SELECT:
            return "select";
        case ProxyGroupType::RELAY:
            return "relay";
        case ProxyGroupType::URL_TEST:
            return "url-test";
        case ProxyGroupType::FALLBACK:
            return "fallback";
        case ProxyGroupType::LOAD_BALANCE:
            return "load-balance";
    }

    throw InvalidValueException("The value of enumerate ProxyGroupType is invalid");
}

std::string ClashSubGenerator::get_provider_type_name(ProviderType providerType) {
    switch (providerType) {
        case ProviderType::FILE:
            return "file";
        case ProviderType::HTTP:
            return "http";
    }

    throw InvalidValueException("The value of enumerate ProviderType is invalid");
}
