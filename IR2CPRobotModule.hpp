#pragma once

#include <vector>

#include "R2CPCommand.hpp"



namespace R2CP {

class IR2CPRobotModule {
public:
    virtual std::vector<R2CPCommand> evaluatereactive(R2CPCommand&) = 0;
    virtual std::vector<R2CPCommand> evaluateactive() = 0;
};

} // End of namespace: R2CP
