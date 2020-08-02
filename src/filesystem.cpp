//
// Created by Kotarou on 2020/4/7.
//
#include <filesystem>
#include <fmt/format.h>

#include "filesystem.h"
#include "exception/file_system_exception.h"

namespace fs = std::filesystem;

bool FileSystem::exists(std::string_view path) {
    return fs::exists(path);
}

bool FileSystem::mkdir(std::string_view path) {
    std::error_code ec;
    auto result = fs::create_directory(path, ec);
    if (ec) {
        throw FileSystemException(fmt::format("An error occurred during creating directory {}", ec.message()));
    }

    return result;
}

void FileSystem::clear_directory(std::string_view path) {
    for (const auto &entry : fs::directory_iterator(path)) {
        std::error_code ec;
        fs::remove_all(entry, ec);
        if (ec) {
            throw FileSystemException(fmt::format("An error occurred during deleting file {}", ec.message()));
        }
    }
}