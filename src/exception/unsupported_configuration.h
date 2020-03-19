//
// Created by Kotarou on 2020/3/19.
//

#ifndef CLASHSUBGENERATOR_UNSUPPORTED_CONFIGURATION_H
#define CLASHSUBGENERATOR_UNSUPPORTED_CONFIGURATION_H

#include "csg_exeption.h"

class UnsupportedConfiguration : public CSGRuntimeException {
public:
    using CSGRuntimeException::CSGRuntimeException;
};

#endif //CLASHSUBGENERATOR_UNSUPPORTED_CONFIGURATION_H
