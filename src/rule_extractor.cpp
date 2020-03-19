//
// Created by Dafan Wang on 2020/3/16.
//

#include <spdlog/spdlog.h>

#include "utils.h"
#include "httpclient.h"
#include "yaml_helper.h"
#include "rule_extractor.h"
#include "exception/missing_key_exception.h"
#include "exception/invalid_yaml_excaption.h"

size_t RuleExtractor::count() const {
    return rules.size();
}

std::set<std::string> RuleExtractor::get_required_proxies() {
    std::set<std::string> proxy_name;
    for (auto entity : rules) {
        auto split = Utils::split(entity.as<std::string>(), ',');
        proxy_name.emplace(split.back());
    }

    const char reserved_keywords[][11] = {"no-resolve", "Final", "DIRECT", "REJECT", "MATCH"};
    for (const auto &key:reserved_keywords) {
        proxy_name.erase(key);
    }

    return proxy_name;
}

YAML::Node RuleExtractor::get() {
    return rules;
}

void RuleExtractor::load(const std::string &uri) {
    auto response = HttpClient::get(uri);
    auto yaml_rules = YAML::Load(response);

    if (!yaml_rules.IsDefined() || yaml_rules.IsScalar()) {
        spdlog::debug("Loaded content: {}", response);
        throw InvalidYamlException("Invalid Yaml file loaded");
    }

    auto key_name = YAMLHelper::search_key(yaml_rules, {"Rule", "rules"});
    rules = yaml_rules[key_name];

    if (rules.size() == 0) {
        spdlog::warn("Empty rule Loaded");
    }
}
