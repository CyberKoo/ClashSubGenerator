//
// Created by Kotarou on 2020/4/7.
//

#ifndef CLASHSUBGENERATOR_FILESYSTEM_H
#define CLASHSUBGENERATOR_FILESYSTEM_H

#include <string>
#include <fstream>
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

    template<class T>
    static bool write(std::string_view file_path, T content) {
        std::ofstream fout(file_path.data());
        if (fout.is_open()) {
            fout << content;
            fout.close();
        } else {
            throw FileSystemException(fmt::format("unable to open file {}", file_path));
        }

        return true;
    }

    template<class T>
    static bool write(std::string_view file_path, std::vector<std::string> &headers, T content) {
        std::ofstream fout(file_path.data());
        if (fout.is_open()) {
            // write headers
            for(const auto &header: headers){
                fout << header << std::endl;
            }
            fout << content;
            fout.close();
        } else {
            throw FileSystemException(fmt::format("unable to open file {}", file_path));
        }

        return true;
    }
};


#endif //CLASHSUBGENERATOR_FILESYSTEM_H
