//
// Created by Kotarou on 2020/3/16.
//

#ifndef CLASHSUBGENERATOR_FILE_NOT_EXISTS_EXCEPTION_H
#define CLASHSUBGENERATOR_FILE_NOT_EXISTS_EXCEPTION_H

#include "csg_exeption.h"

class FileNotExistsException : public CSGRuntimeException {
public:
    using CSGRuntimeException::CSGRuntimeException;
};

#endif //CLASHSUBGENERATOR_FILE_NOT_EXISTS_EXCEPTION_H
