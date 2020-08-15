//
// Created by Kotarou on 2020/8/7.
//

#ifndef CLASHSUBGENERATOR_CONFIG_LOADER_H
#define CLASHSUBGENERATOR_CONFIG_LOADER_H

#include <string>
#include <filesystem>
#include <yaml-cpp/node/node.h>

class Uri;

class ConfigLoader {
public:
    ~ConfigLoader();

    static std::shared_ptr<ConfigLoader> instance();

    std::string load_raw(std::string_view uri, bool local_only = false);

    YAML::Node load_yaml(std::string_view uri, bool local_only = false);

private:
    ConfigLoader();

    std::string cache_file(const Uri &uri);

    std::string load_local_raw(std::string_view path);

    YAML::Node load_local_yaml(std::string_view path);

private:
    std::filesystem::path temporary_dir;
};


#endif //CLASHSUBGENERATOR_CONFIG_LOADER_H
