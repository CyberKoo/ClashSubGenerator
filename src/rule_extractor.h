//
// Created by Dafan Wang on 2020/3/16.
//

#ifndef CLASHSUBGENERATOR_RULE_EXTRACTOR_H
#define CLASHSUBGENERATOR_RULE_EXTRACTOR_H

#include <yaml-cpp/yaml.h>

class RuleExtractor {
public:
    RuleExtractor() = default;

    void load(const std::string &uri);

    YAML::Node get();

    size_t count() const;

    std::set<std::string> required_proxies();

private:
private:
    YAML::Node rules;
};


#endif //CLASHSUBGENERATOR_RULE_EXTRACTOR_H
