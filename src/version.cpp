//
// Created by Kotarou on 2020/4/5.
//

#include <format>

#include <version.h>

std::string get_version() {
    return std::format("ClashSubGenerator {}.{}.{}-{}", CSG_MAJOR, CSG_MINOR, CSG_PATCH, CSG_RELEASE_INFO);
}
