//
// Created by Kotarou on 2020/4/7.
//

#ifndef CLASHSUBGENERATOR_FILESYSTEM_H
#define CLASHSUBGENERATOR_FILESYSTEM_H

#include <string>
#include <filesystem>
#include "exception/file_system_exception.h"

class FileSystem {
public:
    static void error_check(const std::error_code &ec, std::string_view action) {
        if (ec) {
            throw FileSystemException(fmt::format("An error occurred during {}, {}", action, ec.message()));
        }
    }

    template<class T>
    static bool exists(T path) {
        return std::filesystem::exists(path);
    }

    static std::filesystem::path temp_path(std::string_view file_name) {
        return std::filesystem::temp_directory_path() / file_name;
    }

    template<class T>
    static bool mkdir(T path) {
        std::error_code ec;
        auto result = std::filesystem::create_directory(path, ec);
        error_check(ec, "creating directory");

        return result;
    }

    template<class T>
    static void clear_directory(T path) {
        for (const auto &entry : std::filesystem::directory_iterator(path)) {
            std::error_code ec;
            std::filesystem::remove_all(entry, ec);
            error_check(ec, "delete action");
        }
    }

    template<class T>
    static bool rmdir(T path) {
        clear_directory(path);

        std::error_code ec;
        auto result = std::filesystem::remove(path, ec);
        error_check(ec, "delete directory");

        return result;
    }
};


#endif //CLASHSUBGENERATOR_FILESYSTEM_H
