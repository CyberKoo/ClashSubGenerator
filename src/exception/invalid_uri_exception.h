//
// Created by Kotarou on 2020/3/15.
//

#ifndef CLASHSUBGENERATOR_INVALID_URI_EXCEPTION_H
#define CLASHSUBGENERATOR_INVALID_URI_EXCEPTION_H

#include "csg_exeption.h"

class InvalidURIException : public CSGRuntimeException {
public:
    using CSGRuntimeException::CSGRuntimeException;
};

#endif //CLASHSUBGENERATOR_INVALID_URI_EXCEPTION_H
