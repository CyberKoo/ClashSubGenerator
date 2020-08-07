//
// Created by Kotarou on 2020/8/7.
//

#ifndef CLASHSUBGENERATOR_CONFIG_LOADER_H
#define CLASHSUBGENERATOR_CONFIG_LOADER_H

#include <string>
#include <yaml-cpp/node/node.h>

class Uri;

namespace ConfigLoader {
    std::string load_raw(std::string_view uri);

    std::string load_remote_raw(const Uri &uri);

    std::string load_local_raw(std::string_view path);

    YAML::Node load_yaml(std::string_view uri);

    YAML::Node load_local_yaml(std::string_view path);

    YAML::Node load_remote_yaml(const Uri &uri);
};


#endif //CLASHSUBGENERATOR_CONFIG_LOADER_H
