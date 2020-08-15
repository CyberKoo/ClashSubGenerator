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
#include "httpclient.h"
#include "filesystem.h"

std::shared_ptr<ConfigLoader> ConfigLoader::instance() {
    static const std::shared_ptr<ConfigLoader> instance{new ConfigLoader{}};
    return instance;
}

std::string ConfigLoader::load_raw(std::string_view uri, bool local_only, bool use_cache) {
    auto uri_result = Uri::Parse(uri);

    // add support of protocol file://
    if (uri_result.getSchema() == "file" || local_only) {
        SPDLOG_DEBUG("Load local file {}", uri);
        auto path = uri_result.getPath();
        try {
            if (FileSystem::exists(path)) {
                std::fstream fin(path.data());
                return std::string((std::istreambuf_iterator<char>(fin)), std::istreambuf_iterator<char>());
            }
        } catch (std::exception &e) {
            throw FileSystemException(fmt::format("Load file {} filed, error: {}", path, e.what()));
        }

        throw FileSystemException(fmt::format("File {} doesn't exist", path));
    } else {
        return cache_loader(uri_result, use_cache);
    }
}

YAML::Node ConfigLoader::load_yaml(std::string_view uri, bool local_only, bool use_cache) {
    auto uri_result = Uri::Parse(uri);

    // add support of protocol file://
    if (uri_result.getSchema() == "file" || local_only) {
        SPDLOG_DEBUG("load local yaml file {}", uri_result.getBody());

        auto path = uri_result.getBody();
        if (FileSystem::exists(path)) {
            return YAML::LoadFile(path.data());
        }

        throw FileSystemException(fmt::format("File {} doesn't exist", path));
    } else {
        return YAML::Load(cache_loader(uri_result, use_cache));
    }
}

std::string ConfigLoader::cache_loader(const Uri &uri, bool use_cache) {
    if (use_cache) {
        const auto sha1_hash = Hash::sha1(uri.getRawUri());
        if (cache.find(sha1_hash) == cache.end()) {
            // download and cache
            auto content = HttpClient::get(uri);
            cache.emplace(sha1_hash, content);
            SPDLOG_DEBUG("Uri {} downloaded and cached", uri.getRawUri());

            return content;
        } else {
            SPDLOG_DEBUG("Uri {} loaded from cache", uri.getRawUri());
            return cache[sha1_hash];
        }
    } else {
        return HttpClient::get(uri);
    }
}

void ConfigLoader::destroy_cache() {
    auto size = cache.size();
    cache.clear();
    SPDLOG_DEBUG("{} cached data deleted", size);
}
