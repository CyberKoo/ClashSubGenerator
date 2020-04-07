//
// Created by Kotarou on 2020/4/7.
//

#ifndef CLASHSUBGENERATOR_YAML_WRITE_EXCEPTION_H
#define CLASHSUBGENERATOR_YAML_WRITE_EXCEPTION_H

#include "csg_exeption.h"

class FileWriteException : public CSGRuntimeException {
public:
    using CSGRuntimeException::CSGRuntimeException;
};

#endif //CLASHSUBGENERATOR_YAML_WRITE_EXCEPTION_H
