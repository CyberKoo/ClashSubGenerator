//
// Created by Kotarou on 2020/7/9.
//

#ifndef CLASHSUBGENERATOR_INVALID_VALUE_EXCEPTION_H
#define CLASHSUBGENERATOR_INVALID_VALUE_EXCEPTION_H

#include "csg_exeption.h"

class InvalidValueException : public CSGRuntimeException {
public:
    using CSGRuntimeException::CSGRuntimeException;
};

#endif //CLASHSUBGENERATOR_INVALID_VALUE_EXCEPTION_H
