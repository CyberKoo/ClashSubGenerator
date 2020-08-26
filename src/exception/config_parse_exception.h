//
// Created by Kotarou on 2020/8/26.
//

#ifndef CLASHSUBGENERATOR_CONFIG_PARSE_EXCEPTION_H
#define CLASHSUBGENERATOR_CONFIG_PARSE_EXCEPTION_H

#include "csg_exeption.h"

class ConfigParseException : public CSGRuntimeException {
public:
    using CSGRuntimeException::CSGRuntimeException;
};

#endif //CLASHSUBGENERATOR_CONFIG_PARSE_EXCEPTION_H
