//
// Created by Kotarou on 2020/4/7.
//
#include <experimental/filesystem>

#include <fmt/format.h>

#include "filesystem.h"
#include "exception/file_system_exception.h"

namespace fs = std::experimental::filesystem;

bool FileSystem::exists(const std::string &path) {
    return fs::exists(path);
}

bool FileSystem::mkdir(const std::string &path) {
    std::error_code ec;
    auto result = fs::create_directory(path, ec);
    if (ec) {
        throw FileSystemException(fmt::format("An error occurred during creating directory {}", ec.message()));
    }

    return result;
}

void FileSystem::clear_directory(const std::string &path) {
    for (const auto &entry : fs::directory_iterator(path)) {
        std::error_code ec;
        fs::remove_all(entry, ec);
        if (ec) {
            throw FileSystemException(fmt::format("An error occurred during deleting file {}", ec.message()));
        }
    }
}