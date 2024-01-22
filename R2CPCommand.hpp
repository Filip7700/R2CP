#pragma once

#include "R2CPParameter.hpp"

namespace R2CP {

class R2CPCommand {
private:
    uint32_t commandcode;
    std::vector<R2CPParameter> commandparameters;

public:
    R2CPCommand(const uint32_t, const std::vector<R2CPParameter>&);

    // Getters
    uint32_t getcommandcode();
    std::vector<R2CPParameter>& getcommandparameters();
    std::vector<R2CPParameter> getcommandparametersdeepcopy();
    uint32_t getnumberofcommandparameters();
};

} // End of namespace: R2CP
