//
// Created by Kotarou on 2020/3/16.
//

#ifndef CLASHSUBGENERATOR_RULE_EXTRACTOR_H
#define CLASHSUBGENERATOR_RULE_EXTRACTOR_H

#include <set>
#include <yaml-cpp/node/node.h>

class RuleExtractor {
public:
    RuleExtractor() = default;

    ~RuleExtractor() = default;

    void load(std::string_view uri);

    YAML::Node get();

    size_t count() const;

private:
    YAML::Node rules;
};


#endif //CLASHSUBGENERATOR_RULE_EXTRACTOR_H
