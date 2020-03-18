//
// Created by Dafan Wang on 2020/3/16.
//

#ifndef CLASHSUBGENERATOR_INVALID_YAML_EXCAPTION_H
#define CLASHSUBGENERATOR_INVALID_YAML_EXCAPTION_H

#include "csg_exeption.h"

class InvalidYamlException : public CSGRuntimeException {
public:
    using CSGRuntimeException::CSGRuntimeException;
};

#endif //CLASHSUBGENERATOR_INVALID_YAML_EXCAPTION_H
