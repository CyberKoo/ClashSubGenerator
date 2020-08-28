//
// Created by Kotarou on 2020/3/16.
//

#include <spdlog/spdlog.h>

#include "config_loader.h"
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
    auto yaml_rules = ConfigLoader::instance()->load_yaml(uri);

    if (!yaml_rules.IsDefined() || yaml_rules.IsScalar()) {
        SPDLOG_DEBUG("Uri {} loaded", uri);
        throw InvalidYamlException("Invalid Yaml file loaded");
    }

    rules = yaml_rules["rules"];

    if (rules.size() == 0) {
        SPDLOG_WARN("Empty rule loaded");
    }
}
