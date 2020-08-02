//
// Created by Kotarou on 2020/3/16.
//

#include <spdlog/spdlog.h>
#include <yaml-cpp/yaml.h>

#include "httpclient.h"
#include "rule_extractor.h"
#include "exception/missing_key_exception.h"
#include "exception/invalid_yaml_excaption.h"

size_t RuleExtractor::count() const {
    return rules.size();
}

YAML::Node RuleExtractor::get() {
    return rules;
}

void RuleExtractor::load(std::string_view uri) {
    auto response = HttpClient::get(uri);
    auto yaml_rules = YAML::Load(response);

    if (!yaml_rules.IsDefined() || yaml_rules.IsScalar()) {
        spdlog::debug("Loaded content: {}", response);
        throw InvalidYamlException("Invalid Yaml file loaded");
    }

    rules = yaml_rules["rules"];

    if (rules.size() == 0) {
        spdlog::warn("Empty rule Loaded");
    }
}
