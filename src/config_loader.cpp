//
// Created by Kotarou on 2020/8/7.
//
#include <fstream>
#include <fmt/format.h>
#include <yaml-cpp/yaml.h>
#include <spdlog/spdlog.h>

#include "config_loader.h"
#include "uri.h"
#include "hash.h"
#include "utils.h"
#include "httpclient.h"
#include "filesystem.h"
#include "exception/file_system_exception.h"

std::shared_ptr<ConfigLoader> ConfigLoader::instance() {
    static const std::shared_ptr<ConfigLoader> instance{new ConfigLoader{}};
    return instance;
}

ConfigLoader::ConfigLoader() {
    auto instance_id = Utils::get_random_string(15);
    this->temporary_dir = FileSystem::temp_path(instance_id);
    FileSystem::mkdir(this->temporary_dir);
    SPDLOG_DEBUG("Create temporary directory {}", this->temporary_dir.string());
}

ConfigLoader::~ConfigLoader() {
    FileSystem::rmdir(this->temporary_dir);
    SPDLOG_DEBUG("Clear temporary directory {}", this->temporary_dir.string());
}

std::string ConfigLoader::load_raw(std::string_view uri, bool local_only) {
    auto uri_result = Uri::Parse(uri);

    // add support of protocol file://
    if (uri_result.getSchema() == "file" || local_only) {
        SPDLOG_DEBUG("Load local file {}", uri);
        return load_local_raw(uri_result.getPath());
    } else {
        return load_local_raw(cache_file(uri_result));
    }
}

YAML::Node ConfigLoader::load_yaml(std::string_view uri, bool local_only) {
    auto uri_result = Uri::Parse(uri);

    // add support of protocol file://
    if (uri_result.getSchema() == "file" || local_only) {
        SPDLOG_DEBUG("load local yaml file {}", uri_result.getBody());
        return load_local_yaml(uri_result.getBody());
    } else {
        return load_local_yaml(cache_file(uri_result));
    }
}

std::string ConfigLoader::load_local_raw(std::string_view path) {
    if (FileSystem::exists(path)) {
        std::fstream fin(path.data());
        auto content = std::string((std::istreambuf_iterator<char>(fin)), std::istreambuf_iterator<char>());
        fin.close();
        return content;
    }

    throw FileSystemException(fmt::format("File {} doesn't exist", path));
}

YAML::Node ConfigLoader::load_local_yaml(std::string_view path) {
    if (FileSystem::exists(path)) {
        return YAML::LoadFile(path.data());
    }

    throw FileSystemException(fmt::format("File {} doesn't exist", path));
}

std::string ConfigLoader::cache_file(const Uri &uri) {
    auto sha1_hash = Hash::sha1(uri.getRawUri());
    auto temp_path = temporary_dir / sha1_hash;
    if (!FileSystem::exists(temp_path)) {
        // download and cache
        auto file = HttpClient::get(uri);
        std::ofstream fout(temp_path);
        fout << file;
        fout.close();
        SPDLOG_DEBUG("Uri {} downloaded and cached", uri.getRawUri());
    } else {
        SPDLOG_DEBUG("Uri {} loaded from cached", uri.getRawUri());
    }

    // return local address
    return "file://" / temp_path;
}
