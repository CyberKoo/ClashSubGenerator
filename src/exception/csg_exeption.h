//
// Created by Kotarou on 2020/3/15.
//

#ifndef CLASHSUBGENERATOR_CSG_EXEPTION_H
#define CLASHSUBGENERATOR_CSG_EXEPTION_H

#include <stdexcept>

class CSGRuntimeException : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

#endif //CLASHSUBGENERATOR_CSG_EXEPTION_H
