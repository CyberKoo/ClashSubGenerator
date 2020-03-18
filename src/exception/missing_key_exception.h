//
// Created by Kotarou on 2020/3/15.
//

#ifndef CLASHSUBGENERATOR_MISSING_KEY_EXCEPTION_H
#define CLASHSUBGENERATOR_MISSING_KEY_EXCEPTION_H

#include "csg_exeption.h"

class MissingKeyException : public CSGRuntimeException {
public:
    using CSGRuntimeException::CSGRuntimeException;
};

#endif //CLASHSUBGENERATOR_MISSING_KEY_EXCEPTION_H
