//
// Created by Kotarou on 2020/3/15.
//

#ifndef CLASHSUBGENERATOR_REQUEST_FAILURE_EXCEPTION_H
#define CLASHSUBGENERATOR_REQUEST_FAILURE_EXCEPTION_H

#include "csg_exeption.h"

class RequestFailureException : public CSGRuntimeException {
public:
    using CSGRuntimeException::CSGRuntimeException;
};

#endif //CLASHSUBGENERATOR_CLASHSUBGENERATOREXCEPTION_H
