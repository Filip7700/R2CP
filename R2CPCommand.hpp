#pragma once

#include "R2CPParameter.hpp"

namespace R2CP {

class R2CPCommand {
private:
    R2CPCommandCodeType commandcode;
    std::vector<R2CPParameter> commandparameters;

public:
    R2CPCommand(const R2CPCommandCodeType, const std::vector<R2CPParameter>&);

    // Getters
    R2CPCommandCodeType getcommandcode();
    std::vector<R2CPParameter>& getcommandparameters();
    std::vector<R2CPParameter> getcommandparametersdeepcopy();
    uint32_t getnumberofcommandparameters();
};

} // End of namespace: R2CP
