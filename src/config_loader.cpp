//
// Created by Kotarou on 2020/8/7.
//
#include <fstream>
#include <fmt/format.h>
#include <yaml-cpp/yaml.h>
#include <spdlog/spdlog.h>

#include "config_loader.h"
#include "uri.h"
#include "httpclient.h"
#include "filesystem.h"
#include "exception/file_system_exception.h"

std::string ConfigLoader::load_raw(std::string_view uri) {
    auto uri_result = Uri::Parse(uri);

    // add support of protocol file://
    if (uri_result.getSchema() == "file") {
        spdlog::debug("load local file {}", uri);
        return ConfigLoader::load_local_raw(uri_result.getPath());
    } else {
        spdlog::debug("load remote file {}", uri);
        return ConfigLoader::load_remote_raw(uri_result);
    }
}

std::string ConfigLoader::load_local_raw(std::string_view path) {
    if (FileSystem::exists(path)) {
        std::fstream fin(path.data());
        return std::string((std::istreambuf_iterator<char>(fin)), std::istreambuf_iterator<char>());
    }

    throw FileSystemException(fmt::format("File {} doesn't exist", path));
}

std::string ConfigLoader::load_remote_raw(const Uri &uri) {
    return HttpClient::get(uri);
}

YAML::Node ConfigLoader::load_yaml(std::string_view uri) {
    auto uri_result = Uri::Parse(uri);

    // add support of protocol file://
    if (uri_result.getSchema() == "file") {
        spdlog::debug("load local yaml file {}", uri);
        return ConfigLoader::load_local_yaml(uri_result.getPath());
    } else {
        spdlog::debug("load remote yaml file {}", uri);
        return ConfigLoader::load_remote_yaml(uri_result);
    }
}

YAML::Node ConfigLoader::load_remote_yaml(const Uri &uri) {
    auto remote_config = HttpClient::get(uri);
    return YAML::Load(remote_config);
}

YAML::Node ConfigLoader::load_local_yaml(std::string_view path) {
    if (FileSystem::exists(path)) {
        return YAML::LoadFile(path.data());
    }

    throw FileSystemException(fmt::format("File {} doesn't exist", path));
}