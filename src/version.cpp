//
// Created by Kotarou on 2020/4/5.
//

#include <version.h>
#include <fmt/format.h>

std::string get_version() {
    return fmt::format("ClashSubGenerator {}.{}.{}-{}", CSG_MAJOR, CSG_MINOR, CSG_PATCH, CSG_RELEASE_INFO);
}
