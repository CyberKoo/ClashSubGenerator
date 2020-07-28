//
// Created by Kotarou on 2020/4/10.
//

#ifndef CLASHSUBGENERATOR_ENUM_MAPPER_H
#define CLASHSUBGENERATOR_ENUM_MAPPER_H

#include "enumerate.h"

namespace EnumMapper {
    template<class T>
    using enum_mapper_t = std::map<std::string, T>;

    enum_mapper_t<SubscribeType> get_subscribe_type_mapper() {
        return std::map<std::string, SubscribeType>{
                {"CLASH", SubscribeType::CLASH},
                {"OTHER", SubscribeType::OTHER},
        };
    }

    enum_mapper_t<Generator> get_generator_mapper() {
        return std::map<std::string, Generator>{
                {"CONFIG",   Generator::CONFIG},
                {"PROVIDER", Generator::PROVIDER}
        };
    }
}

#endif //CLASHSUBGENERATOR_ENUM_MAPPER_H
