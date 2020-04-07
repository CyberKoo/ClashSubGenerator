//
// Created by Kotarou on 2020/3/16.
//

#ifndef CLASHSUBGENERATOR_FILE_SYSTEM_EXCEPTION_H
#define CLASHSUBGENERATOR_FILE_SYSTEM_EXCEPTION_H

#include "csg_exeption.h"

class FileSystemException : public CSGRuntimeException {
public:
    using CSGRuntimeException::CSGRuntimeException;
};

#endif //CLASHSUBGENERATOR_FILE_SYSTEM_EXCEPTION_H
